<?php
class reflectionException extends reflection_exception {
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
