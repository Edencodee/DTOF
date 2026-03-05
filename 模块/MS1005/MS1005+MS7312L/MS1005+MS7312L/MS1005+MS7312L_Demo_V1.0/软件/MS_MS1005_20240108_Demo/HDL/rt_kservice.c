#include "rt_def.h"

#define RTM_EXPORT(symbol)
/**
 * This function will return the length of a string, which terminate will
 * null character.
 *
 * @param s the string
 *
 * @return the length of string
 */
rt_size_t rt_strlen(const char *s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc) /* nothing */
		;

	return sc - s;
}
RTM_EXPORT(rt_strlen);
