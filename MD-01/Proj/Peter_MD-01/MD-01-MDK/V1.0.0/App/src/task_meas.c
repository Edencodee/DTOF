#include "main.h"
/* Copilot Coding=============================================== */
/* buffer for ISR samples: 1000 samples * STOP_N stops */
uint32_t g_res[1000 * STOP_N];

/* Simple histogram-based measurement handler.
   - builds a small histogram of the 1000 samples,
   - finds the mode, computes mean/std around samples,
   - converts to distance using linear calibration (adjM/adjN),
   - updates g_sreg fields and flags.

   This implementation is intentionally conservative and
   independent of heavy external constants. Replace with
   more advanced fitting (gauss_newton) when calibration
   and mapping are available.
*/
void Meas_HandleHistogram(uint32_t* p)
{
    const int N = 1000 * STOP_N;
    if (p == NULL) return;

    /* find min/max */
    uint32_t minv = 0xFFFFFFFFu, maxv = 0;
    for (int i = 0; i < N; i++) {
        uint32_t v = p[i];
        if (v < minv) minv = v;
        if (v > maxv) maxv = v;
    }

    /* histogram size (clamp to reasonable size) */
    uint32_t range = maxv >= minv ? (maxv - minv + 1) : 1;
    const uint32_t HIST_MAX = 4096u;
    uint32_t offset = minv;
    static uint16_t hist[HIST_MAX];
    memset(hist, 0, sizeof(hist));

    if (range <= HIST_MAX) {
        for (int i = 0; i < N; i++) {
            uint32_t idx = p[i] - offset;
            if (idx < HIST_MAX) hist[idx]++;
        }
        /* find mode */
        uint32_t best = 0, best_idx = 0;
        for (uint32_t i = 0; i < range; i++) {
            if (hist[i] > best) { best = hist[i]; best_idx = i; }
        }

        /* compute weighted mean and std of selected neighborhood */
        int32_t win = 8; /* +- window around mode */
        int32_t lo = (int32_t)best_idx - win;
        int32_t hi = (int32_t)best_idx + win;
        if (lo < 0) lo = 0;
        if ((uint32_t)hi >= range) hi = range - 1;

        double sumw = 0.0, sumx = 0.0, sumxx = 0.0;
        for (int32_t i = lo; i <= hi; i++) {
            uint32_t w = hist[i];
            if (w == 0) continue;
            double x = (double)(offset + i);
            sumw += w;
            sumx += w * x;
            sumxx += w * x * x;
        }

        double pos = 0.0;
        double stddev = 0.0;
        if (sumw > 0.0) {
            pos = sumx / sumw;
            double var = sumxx / sumw - pos * pos;
            stddev = var > 0.0 ? sqrt(var) : 0.0;
        }

        /* convert using linear calibration: dist = pos * adjM + adjN */
        double dist = pos * (double)g_sreg.adjM + (double)g_sreg.adjN;

        /* update registers (dist in units, dist10 = dist * 10) */
        g_sreg.pos = (float)pos;
        g_sreg.dist10 = (int32_t)(dist * 10.0);
        g_sreg.distDsp = g_sreg.dist10;
        g_sreg.distStd = (int32_t)(stddev * (double)g_sreg.adjM * 10.0);

        /* over-range check: compare absolute dist*10 against measDist */
        if ((int32_t)abs(g_sreg.dist10) > (int32_t)g_sreg.measDist) {
            SetFlag(kFlagOverRange);
        } else {
            ClrFlag(kFlagOverRange);
        }

        /* notify update */
        SetFlag(kFlagDistUpdate);
    } else {
        /* range too big; fallback to simple average */
        double sum = 0.0;
        for (int i = 0; i < N; i++) sum += (double)p[i];
        double pos = sum / (double)N;
        double dist = pos * (double)g_sreg.adjM + (double)g_sreg.adjN;
        g_sreg.pos = (float)pos;
        g_sreg.dist10 = (int32_t)(dist * 10.0);
        g_sreg.distDsp = g_sreg.dist10;
        g_sreg.distStd = 0;
        if ((int32_t)abs(g_sreg.dist10) > (int32_t)g_sreg.measDist) SetFlag(kFlagOverRange);
        else ClrFlag(kFlagOverRange);
        SetFlag(kFlagDistUpdate);
    }
}

/* Copilot Coding End=============================================== */
#include "main.h"

