# API

Include `"ggt/green.h"` to use green threads (but see the “teal threads” section
below). Green threads are implemented as coroutines using Duff's Device.

GGT is a header-only library, and requires no compilation. It is released under
the Unlicense and requires no attribution.

Green threads can only use functions designed to be green threads. This has a
few restrictions:

 * Local variables must be stored in a special local-variable object named `l`.
   So, local variables are at `l->foo`, not `foo`.

 * Green-thread functions cannot return values. To return a value, use a return
   parameter.

 * All green-thread functions have the thread itself as their first parameter.

To cope with these restrictions and to correctly handle green threads, all
green-thread functions must be declared using correct macros. Here is a simple
green-thread function named `foo`:

```c
GGT(foo, (ggt_thread_t *thr, int *ret), {
    int *ret;
    int comp;
}, {
    l->ret = ret;
}) {
    *l->ret = 0;
    for (l->comp = 42; l->comp > 0; l->comp--) {
        *l->ret += l->comp;
        GGT_YIELD();
    }

    GGT_END();
}
```

The `GGT` macro takes four arguments: the name, the parameters, the structure of
the local-variable object, and a transfer block for transferring arguments to
local variables. The first parameter must be the thread and must be named `thr`.
This macro is only needed for definition. A standard function declaration (with
`void` return type) is sufficient.

`GGT` declares functions that are usable *in* green threads, but not functions
that are usable *as* green threads (i.e., you cannot spawn a green thread with
them). Use `GGT_E` to make an entry-point function; it takes the same arguments
as `GGT`.

(Note: The distinction between `GGT` and `GGT_E` actually only exists in the
native-thread compatibility layer. If only using green threads, they're the
same.)

All `GGT` and `GGT_E` functions must end with `GGT_END();`.


## Control flow

Generally speaking, a green-thread function is just like any other function,
with a few exceptions:

 * Because green threads are implemented using Duff's device (which is a
   `switch` statement), you must either avoid using `switch` statements in
   green-thread functions entirely, or avoid using `GGT_` macros within `switch`
   statements.

 * Normal functions can be called in a traditional way, but to call a
   green-thread function, you must use `GGT_CALL(target, (args))`. For instance,
   to call the above `foo` function, `GGT_CALL(foo, (thr, &l->resultOfFoo));`.

 * To return, use `GGT_RETURN();`.


## Thread management

To spawn a thread, use `GGT_SPAWN(parent, thread, target, (args))`. For
instance, here's a simple `bar` green-thread function, and a function `baf` to
spawn it:

```c
GGT_E(bar, (ggt_thread_t *thr), {}, {}) {
    printf("I'm bar!\n");
}

GGT(baf, (ggt_thread_t *thr), {
    ggt_thread_t barThread;
}, {}) {
    GGT_SPAWN(*thr, l->barThread, bar, (&l->barThread));
    GGT_JOIN(l->barThread);
}
```

Use `GGT_JOIN` to wait for a thread to terminate, as shown above. `GGT_JOIN`
requires extra information in the thread metadata. To save space, if you don't
need thread joining, you can disable it by setting `GGT_SUPP_JOIN` to `0` before
including `"ggt/green.h"`.

To spawn the initial threads (i.e., to spawn a thread from a non-green-thread
environment), use a `ggt_thread_list_t` as the parent, using `GGT_INIT` to
initialize it. Then, use `ggtRun` to execute green threads.

```c
int main() {
    ggt_thread_list_t list;
    ggt_thread_t thr;
    GGT_INIT(list);
    GGT_SPAWN(list, thr, bar, (&thr));
    ggtRun(&list);
    return 0;
}
```

There is no guarantee that threads only start with `ggtRun`. `ggtRun` merely
continues them. Threads spawned by other threads automatically add themselves to
the thread list.


## Preemption and blocking

A green thread can voluntarily give up the CPU with `GGT_YIELD();`.

To continue yielding until a condition is met, use
`GGT_YIELD_UNTIL(condition);`. Note that this is a polling loop, and so is not
an efficient way to check the condition.

Instead, you can use `GGT_SLEEP(list);` on a thread list to put a thread to
sleep (it will not be schedule for execution), and `GGT_WAKE(list);` to wake all
threads on a list (or, `GGT_WAKE_ONE(list);` to wake only one thread). Make sure
you `GGT_INIT` the thread list before using it.


## Exceptions

Green threads support both a form of exceptions and a form of setjmp/longjmp. By
default, both are enabled, but using them causes thread data structures to be
somewhat larger, so you can disable both with `-DGGT_SUPP_EXCEPTIONS=0
-DGGT_SUPP_SJLJ=0`. Both are implemented in terms of each other, so disabling
only one usually won't save any space. It may be worthwhile to disable one to
make sure the two systems don't conflict.

To set an exception handler, use `GGT_CATCH`:

```c
GGT_CATCH(ex, {
    printf("I caught %p\n", ex);
});
```

Note that `ex` is a local variable, which means it will be lost if the green
thread yields. Make sure to save it to local variable in `l->` if you need to
keep it.

A `GGT_CATCH` block actually acts as both `catch` and `finally`. That is, the
catch block will be run when the function returns *even if there was no throw*.
`ex` will be `NULL` if no exception was thrown. To use it strictly as a catch,
rethrow `ex` at the end of the catch block. Throwing `NULL` is equivalent to
returning.

Catch blocks cannot lexically nest, and there can only be one catch block for a
function. Different functions in the call stack may have different catch blocks,
of course.

To throw, use `GGT_THROW(ex)`. The value thrown must be a `void *`. If the value
is `NULL`, this is equivalent to returning. Otherwise, it's up to you to
interpret what it actually means.

To use a `setjmp`/`longjmp`-like behavior, use a `ggt_jmp_buf_t` as `jmp_buf`,
and use `GGT_SETJMP(jmpbuf, {...});` for `setjmp` (with `{...}` as an actual
code block to run), and `GGT_LONGJMP(jmpbuf)` for `longjmp`. Note that `longjmp`
doesn't pass a value, like normal `longjmp`, because it isn't needed to
distinguish whether the jump occurred or not. Unlike `GGT_CATCH`, the function
will continue after the block in `GGT_SETJMP` if it's jumped into, so make sure
to `GGT_RETURN` if you want to leave the surrounding function.


## Native and “Teal” Threads

GGT supports using the GGT API as a shim for native threads, by including
`"ggt/native.h"` instead of `"ggt/green.h"`.

Or, you can automatically use native threads if supported or green threads
otherwise by including `"ggt/teal.h"`.

You can use green threads on a platform that supports native threads. To use
both at the same time, define `GGT_SUPP_THREADS` to `1` *before* including
`"ggt/green.h"`. If you don't, green threads won't be threadsafe.


## Semaphores

GGT provides semaphores for synchronization. Locks can be implemented in terms
of semaphores.

Include `"ggt/sem.h"` for semaphore support. The type of a semaphore is
`ggt_sem_t`. The API is similar to POSIX semaphores:

`ggt_sem_init(semaphore, value)` initializes a semaphore with the given value.
`semaphore` is a `ggt_sem_t *`.

`ggt_sem_destroy(semaphore)` cleans up a semaphore.

`ggt_sem_post(thread, semaphore)` posts a semaphore. `thread` is a pointer to
the *current* thread.

`GGT_SEM_WAIT(semaphore)` waits for a semaphore. Note that this is a macro, and
can only be used inside of a GGT function.

`ggt_sem_trywait(semaphore)` attempts to wait for a semaphore, but fails if it's
not immediately available. It returns `0` if the semaphore was available, and a
non-zero value if it wasn't.
