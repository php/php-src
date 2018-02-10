<?php
class ReflectionExceptionEx extends ReflectionException {
        function MyException($_errno, $_errmsg) {
                $this->errno = $_errno;
                $this->errmsg = $_errmsg;
        }

        function getErrno() {
                return $this->errno;
        }

        function getErrmsg() {
                return $this->errmsg;
        }
}
?>
