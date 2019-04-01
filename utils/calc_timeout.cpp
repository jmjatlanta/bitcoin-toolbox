#include <iostream>
#include <algorithm>
#include <ctime>

typedef enum {
   unknown,
   minutes,
   hours,
   days
} time_metric;

time_metric get_time_metric(std::string input)
{
   std::transform(input.begin(), input.end(), input.begin(), ::tolower );
   if (input == "minutes")
      return time_metric::minutes;
   if (input == "hours")
      return time_metric::hours;
   if (input == "days")
      return time_metric::days;
   return time_metric::unknown;
}

void print_syntax(std::string cmd)
{
   std::cerr << "Syntax: " << cmd << " # [ minutes | hours | days ]\n";
   exit(1);
}

uint32_t get_time_multiplier(time_metric metric)
{
   switch (metric)
   {
      case time_metric::minutes:
         return 0;
      case time_metric::hours:
         return 60;
      case time_metric::days:
         return 60 * 24;
   }
   print_syntax("calc_timeout");
}

tm operator+ ( tm uct, int span_in_minutes )
{
    // http://www.unix.com/man-page/FreeBSD/3/mktime/
    // ... the original values of the other components are not restricted to their normal ranges, and
    // will be normalized if needed.  For example, October 40 is changed into November 9, a tm_hour
    // of -1 means 1 hour before midnight, tm_mday of 0 means the day preceding the current month,
    // and tm_mon of -2 means 2 months before January of tm_year.
    uct.tm_min += span_in_minutes ;
    const auto t = std::mktime( std::addressof(uct) ) ;

    return *std::gmtime( std::addressof(t) ) ;
}

tm operator+ ( int span_in_minutes, tm uct ) { return uct + span_in_minutes ; }
tm operator- ( tm uct, int span_in_minutes ) {  return uct + -span_in_minutes ; }

int main(int argc, char** argv)
{
   if (argc < 3)
      print_syntax(argv[0]);
   time_metric metric = get_time_metric(argv[2]);
   if (metric == time_metric::unknown)
      print_syntax(argv[0]);
   uint64_t minutes = std::atoi(argv[1]) * get_time_multiplier(metric);

   // calculate the epoch
   const auto now = time(nullptr);
   const auto utc_now = *std::gmtime( std::addressof(now) );
   auto new_time = utc_now + minutes;

   std::cout << std::to_string(mktime(&new_time)) << "\n";

   return 0;
}
