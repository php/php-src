$warnflags = "-Wno-deprecated-declarations"
$warnflags = "" unless try_compile("", $warnflags)

create_makefile("-test-/old_thread_select/old_thread_select")
