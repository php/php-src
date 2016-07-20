# Introduction

This document describes the design of PHP-FPM. It is woefully incomplete, but
at least we are now starting.

## Request stages

FPM defines a set of request stages for tracking a child process' current
status:

* FPM_REQUEST_ACCEPTING: About to call or blocked in accept() on the pool's
  listening socket. The parent considers a child in this stage to be idle.
* FPM_REQUEST_ACCEPTED: Just returned from accept() but not yet in
  READING_HEADERS. This stage exists so that ondemand workers can enter it
  before signaling the parent that they returned from accept(). Without it, a
  race condition exists in which the parent could decide the child was idle
  just *before* it began processing a request, and thus not start listening on
  the pool's socket again.
* FPM_REQUEST_READING_HEADERS: 
* FPM_REQUEST_INFO: 
* FPM_REQUEST_EXECUTING: 
* FPM_REQUEST_END: 
* FPM_REQUEST_FINISHED:


## Scoreboard

### Allocation

FPM maintains a scoreboard data structure that tracks information about the
current state and accumulated activities of workers. Each fpm_worker_pool_s
contains one fpm_scoreboard_s structure, and wp->scoreboard->procs is an array
of wp->config->mp_max_children pointers to fpm_scoreboard_proc_s
structures. All of these structures are stored in shared memory allocated with
mmap(MAP_ANONYMOUS|MAP_SHARED).

The scoreboard proc slots are allocated as needed to children by
fpm_scoreboard_proc_alloc() and _free(), which are non-locked functions that
can only be called by the parent. Each scoreboard proc has a boolean used
flag. The scoreboard maintains an index, free_proc, which indicates that a proc
that might be unused. A scoreboard proc is allocated to a specific child
process by fpm_scoreboard_proc_alloc(). This function first checks whether
scoreboard->procs[scoreboard->free_proc] is unused, and if it is scans through
the array looking for one that is unused. Whichever way it finds one, it marks
that proc used and provides the now-allocated index to its caller. The
free_proc hint is then incremented one (circularly) to suggest a new free
proc. fpm_scoreboard_proc_free() releases a proc slot by memset()ing it to
zero, and setting scoreboard->free_proc to the just-released index.

Just before the parent forks a new child, it allocated a scoreboard proc for
the impending child in fpm_resources_prepare(). Once the child is forked,
fpm_child_resources_use() first frees the scoreboard for all other worker pools
(to prevent accidentally updating them, presumably), and calls
fpm_scoreboard_child_use() which sets the global fpm_scoreboard pointer to the
worker's scoreboard and fpm_scoreboard_i to the child's allocated index in the
worker's scoreboard->procs array. Finally, the child's scoreboard proc is
updated with the child's pid and start time.

A scoreboard proc is atomically locked and unlocked with
fpm_scoreboard_proc_acquire() and _release(). The parent specifies the worker
pool scoreboard and proc index to lock. A child specifies sentinel values that
instruct the functions to use the global fpm_scoreboard and fpm_scoreboard_i
variables.

### Tracking request stages

Each scoreboard proc structure has a field, enum fpm_request_stage_e
request_stage, to track the current stage. Functions such as
fpm_request_accepting() are called from a child to lock the child's scoreboard
proc, update the request stage, set whatever other proc values are appropriate
for the stage, and release the proc.

The parent can inspect a child's current stage via functions like
fpm_request_is_idle(), which accepts a specific fpm_child_s structure,
retrieves its scoreboard proc, and checks the proc's request_stage.

Note that fpm_request_is_idle() explicitly DOES NOT lock the child's proc
structure before reading the request_stage. In Java, this would mean that the
parent had no guarantee of reading the current value. In C, with no specific
memory model, ... all bets are off, I suspect.

## Process management styles

There are three supported process management styles, but they all share a
common flow. In main(), the parent calls fpm_run(). fpm_run() forks initial
worker processes for each pool, if any, with fpm_children_create_initial(), and
calls fpm_event_loop() which runs forever.

The forked children return from fpm_run(), inheriting their listening socket
from the parent. They do a little more initialization, and enter their main loop
calling fcgi_accept_request(). Each call to that function calls accept() on
the socket and then reads and processes a single FastCGI request.

### Static

### Dynamic

### Ondemand

No initial worker processes are started. Instead, fpm_children_create_initial()
installs an FPM_EV_READ event on the pool's listening socket. When a connection
arrives, the event calls fpm_pctl_on_socket_accept(). If there is an idle child
available to run the request, the function does nothing; an idle child is
already waiting in accept() and will respond. Otherwise, if the current system
state allows it, the function forks a new child; if not, the request function
does nothing, and the request stays queued until the next child is
available. Either way, the parent trusts that a newly launched or existing
child will eventually handle the request.

The strategy for deciding when to fork is subtle. The quick summary:

* fpm_pctl_on_socket_accept() always removes the pool's listening socket,
* a child always restores its pool's listening socket after calling accept(),
* a child's first call to accept() is non-blocking,
* a pool's listening socket is always restore when a child exits, and
* a timer restores listening sockets removed due to external system state.

In detail:

Suppose FPM uses a normal level-triggered select()/poll() on the listening
socket. When a connection arrives, fpm_pctl_on_socket_accept() is triggered by
the event system. It finds no idle children, so it forks one, and returns. If
the parent polls again before the child makes it to accept(), the connection is
still pending so fpm_pctl_on_socket_accept() is called again. The child is
idle, so the function returns, but until the child calls accept(), the parent
will spin in this loop. Also, there are a race conditions in which loop occurs
before the child is marked idle (which happens immediately prior to its calling
accept()), in which case the parent will rapidly fork extra processes.

To avoid this, the initial version of ondemand used edge-triggering on the
listening socket (and thus only worked on systems that support epoll() or
kqueue()). This prevents the spin loop because each pending connection is only
returned once, and thus only triggers one call to fpm_pctl_on_socket_accept()
no matter how long the child takes to accept() it. Unfortunately, it causes
different problems. Edge triggering semantics requires that the polling process
read the edge-triggered file until it is empty; in this case, that requires
calling accept() until it returns EAGAIN/EWOULDBLOCK. However, with ondemand,
the parent process polls the listening socket, while the child processes call
accept(), so there is no way for the parent to comply with the edge-triggering
contract. As a result, multiple requests can arrive, generate only a single
edge-triggered event, and launch insufficient processes to handle the requests.

The solution is to use level-triggered polling in a way that avoids spinning
the CPU. The trick is for the parent to remove a pool's listening socket from
its polled set during necessary periods, and to restore it when appropriate. To
facilitate restoring the listening socket, during
fpm_children_create_initial(), the parent creates a per-pool child_accept pipe
and registers an event to call fpm_pctl_on_child_accept() when the pipe is
readable. When invoked, this function reads sizeof(pid_t) from one of the
children and adds the listening socket event back to the polling set.

For removing the listening socket event, there are three cases to
consider. Each time a request arrives and fpm_pctl_on_socket_accept() is
invoked:

* An idle child is available. The parent removes the listening socket. When the
idle child returns from accept(), it writes to the child_accept pipe, restoring
the listening socket.

* No idle child is available, and the parent forks a new child. The parent
removes the listening socket. The child starts and, after calling accept(), it
writes to the child_accept pipe. For this to work, the first call to accept()
must be non-blocking---a sibling process may grab the request first, and the
new child must inform the parent it called accept() so new requests can be
processed whether accept() suceeded or failed.

* No idle child is available, but the parent cannot fork a new child due to
system state (such as already having pm_max_procs children). If there are zero
children and the parent cannot fork one, the parent exits with an error
status. Otherwise, the parent again removes the listening socket, because there
is no point in acting on a pending request when there is no child process to
run it (since level triggering is in place, any pending requests will again
generate a poll event when the socket is restored). The listening socket event
is restored when:

	* A child exits for any reason, since now a new one can possiby be
      started. The SIGCHLD triggers a call to fpm_children_bury(), which looks
      up the child's fpm_child_s record, and writes to the child's child_accept
      pipe regardless of why it exited.

	* A child becomes idle, since it may empty the pending queue. An idle child
      immediately calls accept(), by definition. If there is still a pending
      request, accept() returns, and as per above, the child writes to its
      child_accept pipe. If there is no pending request, it is okay that the
      parent continues ignoring listening socket; the next request to arrive
      will go to an idle child which will write to its child_accept pipe.
