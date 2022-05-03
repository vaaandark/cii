#ifndef EXCEPT_H_
#define EXCEPT_H_

#include <setjmp.h>

struct except {
	const char *reason;
};

struct except_frame {
	struct except_frame *prev;
	jmp_buf env;
	const char *file;
	int line;
	const struct except *exception;
};

enum {
	EXCEPT_ENTERED = 0,
	EXCEPT_RAISEED,
	EXCEPT_HANDLED,
	EXCEPT_FINALIZED
};

extern struct except_frame *except_stack;

void except_raise(const struct except *e, const char *file, int line);

#define TRY do { \
	volatile int except_flag; \
	struct except_frame except_frame; \
	except_frame.prev = except_stack; \
	except_stack = &except_frame; \
	except_flag = setjmp(except_frame.env); \
	if (except_flag == EXCEPT_ENTERED) {

#define EXCEPT(e) \
		if (except_flag == EXCEPT_ENTERED) \
			except_stack = except_stack->prev; \
	} else if (except_frame.exception == &(e)) { \
		except_flag = EXCEPT_HANDLED;

#define ELSE \
		if (except_flag == EXCEPT_ENTERED) \
			except_stack = except_stack->prev; \
	} else { \
		except_flag = EXCEPT_HANDLED;

#define FINALLY \
		if (except_flag == EXCEPT_ENTERED) \
			except_stack = except_stack->prev; \
	} { \
		if (except_flag == EXCEPT_ENTERED) \
			except_flag = EXCEPT_FINALIZED;

#define END_TRY \
		if (except_flag == EXCEPT_ENTERED) \
			except_stack = except_stack->prev; \
	} if (except_flag == EXCEPT_RAISEED) RERAISE; \
} while (0)

#define RAISE(e) except_raise(&(e), __FILE__, __LINE__)

#define RERAISE except_raise(except_frame.exception, except_frame.file, except_frame.line)

#define RETURN switch (except_stack = except_stack->prev , 0) default: return


#endif