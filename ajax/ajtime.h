#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajtime_h
#define ajtime_h
#include <time.h>

/* @data AjPTime **************************************************************
**
** AJAX time and data data structure. The same structure is used for both.
**
** @alias AjPDate
**
** @attr time [struct tm] Time in C standard structure
** @attr format [const char*] Format string if specified
** @attr uppercase [AjBool] Convert to upper case on output
** @attr Padding [char[4]] Padding to alignment boundary
** @@
******************************************************************************/

typedef struct AjSTime {
  struct tm time;
  const char *format;
  AjBool uppercase;
  char Padding[4];
} AjOTime, AjODate;

#define AjPTime AjOTime*
#define AjPDate AjODate*




/*
** Prototype definitions
*/

void          ajTimeExit(void);
AjBool        ajTimeLocal(const time_t timer, AjPTime thys);
AjPTime       ajTimeToday (void);
AjPTime       ajTimeTodayF (const char* timefmt);
const AjPTime ajTimeTodayRef (void);
const AjPTime ajTimeTodayRefF (const char* timefmt);
void          ajTimeTrace (const AjPTime thys);
AjPTime       ajTimeSet(const char *timefmt,
			ajint mday, ajint mon, ajint year) ;
AjBool        ajTimeSetS(AjPTime thys, const char* timestr);
AjPTime       ajTimeNew(void);
AjPTime       ajTimeNewTime(const AjPTime thys);
void          ajTimeDel(AjPTime *thys);
time_t        ajTimeMake(const AjPTime thys);

/*
** End of prototype definitions
*/

#endif

#ifdef __cplusplus
}
#endif
