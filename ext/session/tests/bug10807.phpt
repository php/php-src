--TEST--
Bug #10807 (session_regenerate_id with custom handler and use_strict_mode generates three new session ids)
--EXTENSIONS--
session
--INI--
html_errors=0
session.save_handler=files
session.use_strict_mode=1
--FILE--
<?php

/**
 * This class will return a number for the session id, incrementing each time a new
 * session is generated
 */
class Bug10807SessionHandler implements SessionHandlerInterface, SessionIdInterface {

	private bool $fail_validation;
	private int $sid_counter;

	public function __construct() {
		$this->fail_validation = FALSE;
		$this->sid_counter = 1;

		session_set_save_handler($this, FALSE);
	}

	public function setFailValidation() {
		$this->fail_validation = TRUE;
	}

	public function create_sid() : string {
		return strval($this->sid_counter++);
	}

	public function validateId(string $id) : bool {

		if ($this->fail_validation && intval($id)< 4) {
			return FALSE;
		}

		return TRUE;

	}

	public function close(): bool { return TRUE; }
	public function destroy(string $id): bool { return TRUE; }
	public function gc(int $max_lifetime): int|false { return TRUE; }
	public function open(string $path, string $name): bool { return TRUE; }
	public function read(string $id): string|false { return ''; }
	public function write(string $id, string $data): bool { return TRUE; }

}

ob_start();

$save_handler = new Bug10807SessionHandler();
session_start(); //session id = 1
session_regenerate_id(); //session id = 2
var_dump(session_id());

$save_handler->setFailValidation(); 
session_regenerate_id(); //should invoke create_sid twice as session id 3 will be invalid
var_dump(session_id());

?>
--EXPECT--
string(1) "2"
string(1) "4"
