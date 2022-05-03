#include "assert.h"
#include "except.h"
#include "stdio.h"
#include "stdlib.h"

#define TEST 3

struct except_frame *except_stack = NULL;

void except_raise(const struct except *e, const char *file ,int line)
{
	struct except_frame *p = except_stack;

	assert(e);
	if (!p) {
		fprintf(stderr, "Uncaught exception");
		if (e->reason)
			fprintf(stderr, " %s", e->reason);
		else
			fprintf(stderr, " at %p", e);
		if (file && line > 0)
			fprintf(stderr, " raised at %s: %d\n", file, line);
		fprintf(stderr, "aborting...\n");
		fflush(stderr);
		abort();
	}
	p->exception = e;
	p->file = file;
	p->line = line;
	except_stack = except_stack->prev;
	longjmp(p->env, EXCEPT_RAISEED);
}

#ifdef TEST

struct except allocate_fail = {"Allocate failed"};
struct except div_by_zero = {"divided by 0"};
struct except unknown_exception = {"unknown exception"};

void except_test(int a)
{
	if (a == 0)
		RAISE(allocate_fail);
	else if (a == 1)
		RAISE(div_by_zero);
	else
		RAISE(unknown_exception);
}

int main(void)
{
#if TEST == 1

	TRY
		printf("Hello world!\n");
	FINALLY
	END_TRY;

#elif TEST == 2

	TRY
		printf("Hello world!\n");
	END_TRY;

#elif TEST == 3

	TRY
		except_test(0);
//		except_test(1);
//		except_test(2);
	EXCEPT(allocate_fail)
		printf("catch a exception of %s\n", allocate_fail.reason);
	EXCEPT(div_by_zero)
		printf("catch a exception of %s\n", div_by_zero.reason);
	ELSE
		printf("catch a exception of %s\n", unknown_exception.reason);
		RERAISE;
	END_TRY;
	
#endif
	return 0;
}

#endif

#undef TEST