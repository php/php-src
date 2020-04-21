<?php

/**
 * @generate-function-entries
 */

namespace NSMrSpoocy {

    interface InterfaceInNamespace
    {
        /** @return int */
        public function interfaceMethod(): int;

        public static function staticInterfaceMethod(string $packet): void;
    }

    function functionInNamespace(): void  {

    }

    /**
     * @deprecated
     */
    function deprecatedFunctionInNamespace(): void  {

    }

    class ClassInNamespace {

        public function classMethodInNamespace(): string {}

        /**
         * @return bool
         * @deprecated
         */
        public function deprecatedClassMethodInNamespace(): string {}
    }
}

namespace {

    function functionInGlobalNamespace(): void  {

    }

    /**
     * @return bool
     * @deprecated
     */
    function deprecatedFunctionInGlobalNamespace(): bool {}

    class ClassInGlobalNamespace {

        public function classMethodInGlobalNamespace(): void {}

        public function classMethodInGlobalNamespaceWithParam(): bool {}
    }

}

namespace NSMrSpoocy\NSAnother {


    class NestedNamespaceClass
    {

        public function classMethodInNestedNamespace(): bool {}
    }

    function functionInNestedNamespace(): bool {}

    function functionInNestedNamespaceWithParam(string $param): bool {}
}

?>