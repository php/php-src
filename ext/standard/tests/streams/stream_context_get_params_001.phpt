--TEST--
stream_context_get_params()
--FILE--
<?php

$ctx = stream_context_create();
var_dump($ctx);
var_dump(stream_context_get_params($ctx));

var_dump(stream_context_set_option($ctx, "foo","bar","baz"));
var_dump(stream_context_get_params($ctx));

var_dump(stream_context_set_params($ctx, array("notification" => "stream_notification_callback")));
var_dump(stream_context_get_params($ctx));

var_dump(stream_context_set_params($ctx, array("notification" => array("stream","notification_callback"))));
var_dump(stream_context_get_params($ctx));

var_dump(stream_context_get_params($ctx));
var_dump(stream_context_get_options($ctx));
var_dump(stream_context_get_params($ctx));
var_dump(stream_context_get_options($ctx));

?>
--EXPECTF--
resource(%d) of type (stream-context)
array(1) {
  [%u|b%"options"]=>
  array(0) {
  }
}
bool(true)
array(1) {
  [%u|b%"options"]=>
  array(1) {
    [%u|b%"foo"]=>
    array(1) {
      [%u|b%"bar"]=>
      %unicode|string%(3) "baz"
    }
  }
}
bool(true)
array(2) {
  [%u|b%"notification"]=>
  %unicode|string%(28) "stream_notification_callback"
  [%u|b%"options"]=>
  array(1) {
    [%u|b%"foo"]=>
    array(1) {
      [%u|b%"bar"]=>
      %unicode|string%(3) "baz"
    }
  }
}
bool(true)
array(2) {
  [%u|b%"notification"]=>
  array(2) {
    [0]=>
    %unicode|string%(6) "stream"
    [1]=>
    %unicode|string%(21) "notification_callback"
  }
  [%u|b%"options"]=>
  array(1) {
    [%u|b%"foo"]=>
    array(1) {
      [%u|b%"bar"]=>
      %unicode|string%(3) "baz"
    }
  }
}
array(2) {
  [%u|b%"notification"]=>
  array(2) {
    [0]=>
    %unicode|string%(6) "stream"
    [1]=>
    %unicode|string%(21) "notification_callback"
  }
  [%u|b%"options"]=>
  array(1) {
    [%u|b%"foo"]=>
    array(1) {
      [%u|b%"bar"]=>
      %unicode|string%(3) "baz"
    }
  }
}
array(1) {
  [%u|b%"foo"]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "baz"
  }
}
array(2) {
  [%u|b%"notification"]=>
  array(2) {
    [0]=>
    %unicode|string%(6) "stream"
    [1]=>
    %unicode|string%(21) "notification_callback"
  }
  [%u|b%"options"]=>
  array(1) {
    [%u|b%"foo"]=>
    array(1) {
      [%u|b%"bar"]=>
      %unicode|string%(3) "baz"
    }
  }
}
array(1) {
  [%u|b%"foo"]=>
  array(1) {
    [%u|b%"bar"]=>
    %unicode|string%(3) "baz"
  }
}
