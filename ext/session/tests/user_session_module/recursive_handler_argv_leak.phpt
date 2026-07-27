--TEST--
ps_call_handler() releases argv when a recursive save-handler call is rejected
--INI--
session.save_path=
session.name=PHPSESSID
--EXTENSIONS--
session
--FILE--
<?php
class H extends SessionHandler {
    public bool $tripped = false;
    public function write($id, $data): bool {
        if (!$this->tripped) {
            $this->tripped = true;
            session_destroy();
        }
        return true;
    }
}

session_set_save_handler(new H, true);
session_start();
$_SESSION['x'] = 1;
session_write_close();
echo "done\n";
?>
--EXPECTF--
Warning: session_destroy(): Cannot call session save handler in a recursive manner in %s on line %d

Warning: session_destroy(): Session object destruction failed in %s on line %d
done
