/* tst-strptime2 - Test strptime %z timezone offset specifier.  */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


/* Dummy string is used to match strptime's %s specifier.  */

static const char const *dummy_string = "1113472456";

/* buffer_size contains the maximum test string length, including
   trailing NUL.  */

enum
{
  buffer_size = 20,
};

/* Verbose execution, set with --verbose command line option.  */

static bool verbose;

/* NUmber of tests run with failing and passing results.  */

static unsigned int nfail;
static unsigned int npass;


/* mkbuf - Write a test string for strptime with the specified time
   value and number of digits into the supplied buffer, and return
   the expected strptime test result.

   The test string, buf, is written with the following content:
     a dummy string matching strptime "%s" format specifier,
     whitespace matching strptime " " format specifier, and
     timezone string matching strptime "%z" format specifier.

   Note that a valid timezone string contains the following fields:
     Sign field consisting of a '+' or '-' sign,
     Hours field in two decimal digits, and
     optional Minutes field in two decimal digits.

   This function may write test strings with minutes values outside
   the valid range 00-59.  These are invalid strings and useful for
   testing strptime's rejection of invalid strings.

   The ndigits parameter is used to limit the number of timezone
   string digits to be written and may range from 0 to 4.  Note that
   only 2 and 4 digit strings are valid input to strptime; strings
   with 0, 1 or 3 digits are invalid and useful for testing strptime's
   rejection of invalid strings.

   This function returns the behavior expected of strptime resulting
   from parsing the the test string.  For valid strings, the function
   returns the expected tm_gmtoff value.  For invalid strings,
   LONG_MAX is returned.  LONG_MAX indicates the expectation that
   strptime will return NULL; for example, if the number of digits
   are not correct, or minutes part of the time is outside the valid
   range of 00 to 59.  */

static long int
mkbuf (char *buf, bool neg, unsigned int hhmm, size_t ndigits)
{
  const int mm_max = 59;
  char sign = neg ? '-' : '+';
  int i;
  unsigned int hh = hhmm / 100;
  unsigned int mm = hhmm % 100;
  long int expect = LONG_MAX;

  i = sprintf (buf, "%s %c", dummy_string, sign);
  snprintf (buf + i, ndigits + 1, "%04u", hhmm);

  if (mm <= mm_max && (ndigits == 2 || ndigits == 4))
    {
      long int tm_gmtoff = hh * 3600 + mm * 60;

      expect = neg ? -tm_gmtoff : tm_gmtoff;
    }

  return expect;
}


/* Write a description of expected or actual test result to stdout.  */

static void
describe (bool string_valid, long int tm_gmtoff)
{
  if (string_valid)
    printf ("valid, tm.tm_gmtoff %ld", tm_gmtoff);
  else
    printf ("invalid, return value NULL");
}


/* Using buffer, test strptime against expected result and report
   results to stdout.  Return 1 if test failed, 0 if passed.  */

static int
compare (const char *buf, long int expect)
{
  struct tm tm;
  char *retval;
  long int test_result;
  bool fail;

  retval = strptime (buf, "%s %z", &tm);
  test_result = retval ? tm.tm_gmtoff : LONG_MAX;
  fail = test_result != expect;

  if (fail || verbose)
    {
      bool expect_string_valid = (expect == LONG_MAX) ? 0 : 1;

      printf ("%s: input \"%s\", expected: ", fail ? "FAIL" : "PASS", buf);
      describe (expect_string_valid, expect);

      if (fail)
	{
	  bool test_string_valid = retval ? 1 : 0;

	  printf (", got: ");
	  describe (test_string_valid, test_result);
	}

      printf ("\n");
    }

  nfail += fail;
  npass += !fail;

  return fail;
}


static int
do_test (void)
{
  char buf[buffer_size];
  long int expect;
  int result = 0;
  unsigned int ndigits;
  unsigned int step;
  unsigned int hhmm;
  unsigned int neg;

  /* Create and test input string with no sign and four digits input
     (invalid format).  */

  sprintf (buf, "%s  1030", dummy_string);
  expect = LONG_MAX;
  result |= compare (buf, expect);

  /* Create and test input strings with positive and negative sign and digits:
     0 digits (invalid format),
     1 digit (invalid format),
     2 digits (valid format),
     3 digits (invalid format),
     4 digits (valid format if and only if minutes is in range 00-59,
       otherwise invalid).
     If format is valid, the returned tm_gmtoff is checked.  */

  for (ndigits = 0, step = 10000; ndigits <= 4; ndigits++, step /= 10)
    for (hhmm = 0; hhmm <= 9999; hhmm += step)
      for (neg = 0; neg <= 1; neg++)
	{
	  expect = mkbuf (buf, neg, hhmm, ndigits);
	  result |= compare (buf, expect);
	}

  if (nfail || verbose)
    printf ("%s: %u input strings: %u fail, %u pass\n",
	    nfail ? "FAIL" : "PASS", nfail + npass, nfail, npass);

  return result;
}

/* Add a "--verbose" command line option to test-skeleton.c.  */

#define OPT_VERBOSE 10000

#define CMDLINE_OPTIONS \
  { "verbose", no_argument, NULL, OPT_VERBOSE, },

#define CMDLINE_PROCESS \
  case OPT_VERBOSE: \
    { \
      extern bool verbose; \
      verbose = 1; \
    } \
    break;

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
