/**********************************************************************

  ruby/vm.h -

  $Author$
  created at: Sat May 31 15:17:36 2008

  Copyright (C) 2008 Yukihiro Matsumoto

**********************************************************************/

#ifndef RUBY_VM_H
#define RUBY_VM_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* Place holder.
 *
 * We will prepare VM creation/control APIs on 1.9.2 or later.
 * If you have an interest about it, please see mvm branch.
 * http://svn.ruby-lang.org/cgi-bin/viewvc.cgi/branches/mvm/
 */

/* VM type declaration */
typedef struct rb_vm_struct ruby_vm_t;

/* core API */
int ruby_vm_destruct(ruby_vm_t *vm);

/**
 * ruby_vm_at_exit registers a function _func_ to be invoked when a VM
 * passed away.  Functions registered this way runs in reverse order
 * of registration, just like END {} block does.  The difference is
 * its timing to be triggered. ruby_vm_at_exit functions runs when a
 * VM _passed_ _away_, while END {} blocks runs just _before_ a VM
 * _is_ _passing_ _away_.
 *
 * You cannot register a function to another VM than where you are in.
 * So where to register is intuitive, omitted.  OTOH the argument
 * _func_ cannot know which VM it is in because at the time of
 * invocation, the VM has already died and there is no execution
 * context.  The VM itself is passed as the first argument to it.
 *
 * @param[in] func the function to register.
 */
void ruby_vm_at_exit(void(*func)(ruby_vm_t *));

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* RUBY_VM_H */
