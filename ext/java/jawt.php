<?

  // This example is only intented to be run as a CGI.

  $frame = new Java("java.awt.Frame", "Zend");
  $button = new Java("java.awt.Button", "Hello Java world!");
  $frame->add("North", $button);
  $frame->validate();
  $frame->pack();
  $frame->visible = True;

  $thread = new Java("java.lang.Thread");
  $thread->sleep(10000);

  $frame->dispose();

  // Odd behavior noted with Sun JVMs:
  //
  //   1) $thread->destroy() will fail with a NoSuchMethodError exception.
  //   2) The call to (*jvm)->DestroyJVM(jvm) made when PHP terminates
  //      will hang, unless _BOTH_ the calls to pack and setVisible above
  //      are removed.
  //
  //  Even more odd: both effects are seen with a 100% Java implementation
  //  of the above!

?>
