#ifndef OAK_RUN_LOOP_H_7Q2KD1VX
#define OAK_RUN_LOOP_H_7Q2KD1VX

#include <CoreFoundation/CoreFoundation.h>

namespace oak
{
	// A run loop without sources or timers returns from CFRunLoopRun…() at
	// once, so a loop that waits for a callback by calling it repeatedly
	// spins. On a GCD worker thread the spinning thread also keeps the pool
	// from starting the blocks that would deliver the callback, and the wait
	// never ends. While this object lives, the current run loop holds a timer
	// that never fires, which makes it sleep until it is woken up.
	struct keep_run_loop_waiting_t
	{
		keep_run_loop_waiting_t () : _run_loop(CFRunLoopGetCurrent())
		{
			_timer = CFRunLoopTimerCreateWithHandler(kCFAllocatorDefault, CFAbsoluteTimeGetCurrent() + 1.0e10, 0, 0, 0, ^(CFRunLoopTimerRef){ });
			CFRunLoopAddTimer(_run_loop, _timer, kCFRunLoopCommonModes);
		}

		~keep_run_loop_waiting_t ()
		{
			CFRunLoopRemoveTimer(_run_loop, _timer, kCFRunLoopCommonModes);
			CFRelease(_timer);
		}

		keep_run_loop_waiting_t (keep_run_loop_waiting_t const&) = delete;
		keep_run_loop_waiting_t& operator= (keep_run_loop_waiting_t const&) = delete;

	private:
		CFRunLoopRef _run_loop;
		CFRunLoopTimerRef _timer;
	};

} /* oak */

#endif /* end of include guard: OAK_RUN_LOOP_H_7Q2KD1VX */
