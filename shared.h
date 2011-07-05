/**
 * Contains definitions used both by send and recv.
 */

#ifndef _SHARED_H_
#define _SHARED_H_

//trace off
//#define TRACE(fmt, args...) ((void) 0)
//trace on
//#define TRACE(fmt, args...) fprintf(stdout, "%s: " fmt, __func__, ##args)
#define TRACE ERROR

#define ERROR(fmt, args...) fprintf(stderr, "%s: " fmt, __func__, ##args)
#define FATAL(fmt, args...) do { ERROR(fmt, ##args); exit(1); } while (0) 

#endif
