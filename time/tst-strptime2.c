#include <limits.h>
#include <stdio.h>
#include <time.h>

/* Dummy string is used to match strptime's %s specifier.  */

static const char const *dummy_string = "1113472456";

/* buffer_size contains the maximum test string length,
   including trailing NUL.  */

enum {
  buffer_size = 20,
};


/*
   Write a string into the supplied buffer, containing a dummy string,
   + or - sign, two hour digits, optionally two minutes digits,
   and trailing NUL.

   Also, calculate and return expected results for this value.  If the
   input is valid then the expected gmtoffset is returned. If the
   value is invalid input to strptime, then LONG_MAX is returned.
   LONG_MAX indicates the expectation that strptime will return NULL;
   for example, if the number of digits are not correct, or minutes
   part of the time is outside the valid range of 00 to 59.
   */

static int
mkbuf (char *buf, int hhmm_signed, size_t ndigits)
{
  const int mm_max = 59;
  int neg = hhmm_signed < 0 ? 1 : 0;
  unsigned int hhmm = neg ? -hhmm_signed : hhmm_signed;
  unsigned int hh = hhmm / 100;
  unsigned int mm = hhmm % 100;
  char sign = neg ? '-' : '+';
  int gmtoff_signed = LONG_MAX;

  switch (ndigits)
    {
      case 0:
	sprintf(buf, "%s %c", dummy_string, sign);
	break;

      case 1:
      case 2:
	sprintf(buf, "%s %c%0*u", dummy_string, sign, ndigits, hh);
	break;

      case 3:
      case 4:
	sprintf(buf, "%s %c%0*u", dummy_string, sign, ndigits, hhmm);
	break;

      default:
	sprintf(buf, "%s %c%u", dummy_string, sign, hhmm);
	break;

    }

  if (mm <= mm_max && (ndigits == 2 || ndigits == 4))
    {
      int gmtoff = hh * 3600 + mm * 60;

      gmtoff_signed = neg ? -gmtoff : gmtoff;
    }

  return gmtoff_signed;
}


/* Using buffer, test strptime against expected gmtoff result.  */

static int
compare (const char *buf, int gmtoff)
{
  int result = 0;
  struct tm tm;

  if (strptime (buf, "%s %z", &tm) == NULL)
    {
      if (gmtoff != LONG_MAX)
	{
	  printf ("%s: strptime unexpectedly failed\n", buf);
	  result = 1;
	}
    }
  else if (tm.tm_gmtoff != gmtoff)
    {
      printf ("%s: tm_gmtoff is %ld\n", buf, (long int) tm.tm_gmtoff);
      result = 1;
    }

  return result;
}


static int
do_test (void)
{
  const int hhmm_min = -9999;
  const int hhmm_max = 9999;
  const int hh_min = -99;
  const int hh_max = 99;
  int result = 0;
  int gmtoff;
  char buf[buffer_size];

  /* Test sign missing, four digits string (invalid format).  */

  sprintf(buf, "%s  1030", dummy_string);
  gmtoff = LONG_MAX;
  result |= compare(buf, gmtoff);

  /* Test sign and no digits strings (invalid format).  */

  gmtoff = mkbuf(buf, 0, 0);
  result |= compare(buf, gmtoff);

  /* Test sign and one digit strings (invalid format).  */

  for (int hh = hh_min / 10; hh <= hh_max / 10; hh++)
    {
      gmtoff = mkbuf(buf, hh * 100, 1);
      result |= compare(buf, gmtoff);
    }

  /* Test sign and two digits strings (valid format).  */

  for (int hh = hh_min; hh <= hh_max; hh++)
    {
      gmtoff = mkbuf(buf, hh * 100, 2);
      result |= compare(buf, gmtoff);
    }

  /* Test sign and three digits strings (invalid format).  */

  for (int hhmm = hhmm_min / 10; hhmm <= hhmm_max / 10; hhmm++)
    {
      gmtoff = mkbuf(buf, hhmm, 1);
      result |= compare(buf, gmtoff);
    }

  /* Test sign and four digits strings. This includes cases
     where minutes are in the range 0 to 59 (valid), and
     where minutes are in the range 60 to 99 (invalid).  */

  for (int hhmm = hhmm_min; hhmm <= hhmm_max; hhmm++)
    {
      gmtoff = mkbuf(buf, hhmm, 4);
      result |= compare(buf, gmtoff);
    }

  return result;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
