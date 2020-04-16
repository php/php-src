#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\Node;
use PhpParser\Node\Expr;
use PhpParser\Node\Name;
use PhpParser\Node\Stmt;
use PhpParser\Node\Stmt\Class_;
use PhpParser\PrettyPrinter\Standard;
use PhpParser\PrettyPrinterAbstract;

error_reporting(E_ALL);

try {
    initPhpParser();
} catch (Exception $e) {
    echo "{$e->getMessage()}\n";
    exit(1);
}

class CustomPrettyPrinter extends Standard
{
    protected function pName_FullyQualified(Name\FullyQualified $node) {
        return implode('\\', $node->parts);
    }
}

function processDirectory(string $dir) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            processStubFile($pathName);
        }
    }
}

function processStubFile(string $stubFile) {
    $arginfoFile = str_replace('.stub.php', '', $stubFile) . '_arginfo.h';

    try {
        $fileInfo = parseStubFile($stubFile);
        $arginfoCode = generateArgInfoCode($fileInfo);
        file_put_contents($arginfoFile, $arginfoCode);
    } catch (Exception $e) {
        echo "In $stubFile:\n{$e->getMessage()}\n";
        exit(1);
    }
}

class SimpleType {
    /** @var string */
    public $name;
    /** @var bool */
    public $isBuiltin;

    public function __construct(string $name, bool $isBuiltin) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
    }

    public static function fromNode(Node $node) {
        if ($node instanceof Node\Name) {
            assert($node->isFullyQualified());
            return new SimpleType($node->toString(), false);
        }
        if ($node instanceof Node\Identifier) {
            return new SimpleType($node->toString(), true);
        }
        throw new Exception("Unexpected node type");
    }

    public function isNull() {
        return $this->isBuiltin && $this->name === 'null';
    }

    public function toTypeCode() {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
        case "bool":
            return "_IS_BOOL";
        case "int":
            return "IS_LONG";
        case "float":
            return "IS_DOUBLE";
        case "string":
            return "IS_STRING";
        case "array":
            return "IS_ARRAY";
        case "object":
            return "IS_OBJECT";
        case "void":
            return "IS_VOID";
        case "callable":
            return "IS_CALLABLE";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeMask() {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
        case "null":
            return "MAY_BE_NULL";
        case "false":
            return "MAY_BE_FALSE";
        case "bool":
            return "MAY_BE_BOOL";
        case "int":
            return "MAY_BE_LONG";
        case "float":
            return "MAY_BE_DOUBLE";
        case "string":
            return "MAY_BE_STRING";
        case "array":
            return "MAY_BE_ARRAY";
        case "object":
            return "MAY_BE_OBJECT";
        case "callable":
            return "MAY_BE_CALLABLE";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toEscapedName(): string {
        return str_replace('\\', '\\\\', $this->name);
    }

    public function equals(SimpleType $other) {
        return $this->name === $other->name
            && $this->isBuiltin === $other->isBuiltin;
    }
}

class Type {
    /** @var SimpleType[] $types */
    public $types;

    public function __construct(array $types) {
        $this->types = $types;
    }

    public static function fromNode(Node $node) {
        if ($node instanceof Node\UnionType) {
            return new Type(array_map(['SimpleType', 'fromNode'], $node->types));
        }
        if ($node instanceof Node\NullableType) {
            return new Type([
                SimpleType::fromNode($node->type),
                new SimpleType('null', true),
            ]);
        }
        return new Type([SimpleType::fromNode($node)]);
    }

    public function isNullable(): bool {
        foreach ($this->types as $type) {
            if ($type->isNull()) {
                return true;
            }
        }
        return false;
    }

    public function getWithoutNull(): Type {
        return new Type(array_filter($this->types, function(SimpleType $type) {
            return !$type->isNull();
        }));
    }

    public function tryToSimpleType(): ?SimpleType {
        $withoutNull = $this->getWithoutNull();
        if (count($withoutNull->types) === 1) {
            return $withoutNull->types[0];
        }
        return null;
    }

    public function tryToRepresentableType(): ?RepresentableType {
        $classType = null;
        $builtinTypes = [];
        foreach ($this->types as $type) {
            if ($type->isBuiltin) {
                $builtinTypes[] = $type;
            } else if ($classType === null) {
                $classType = $type;
            } else {
                // We can only represent a single class type.
                return null;
            }
        }
        return new RepresentableType($classType, $builtinTypes);
    }

    public static function equals(?Type $a, ?Type $b): bool {
        if ($a === null || $b === null) {
            return $a === $b;
        }

        if (count($a->types) !== count($b->types)) {
            return false;
        }

        for ($i = 0; $i < count($a->types); $i++) {
            if (!$a->types[$i]->equals($b->types[$i])) {
                return false;
            }
        }

        return true;
    }
}

class RepresentableType {
    /** @var ?SimpleType $classType */
    public $classType;
    /** @var SimpleType[] $builtinTypes */
    public $builtinTypes;

    public function __construct(?SimpleType $classType, array $builtinTypes) {
        $this->classType = $classType;
        $this->builtinTypes = $builtinTypes;
    }

    public function toTypeMask(): string {
        return implode('|', array_map(function(SimpleType $type) {
            return $type->toTypeMask();
        }, $this->builtinTypes));
    }
}

class ArgInfo {
    const SEND_BY_VAL = 0;
    const SEND_BY_REF = 1;
    const SEND_PREFER_REF = 2;

    /** @var string */
    public $name;
    /** @var int */
    public $sendBy;
    /** @var bool */
    public $isVariadic;
    /** @var Type|null */
    public $type;
    /** @var string|null */
    public $defaultValue;

    public function __construct(string $name, int $sendBy, bool $isVariadic, ?Type $type, ?string $defaultValue) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
        $this->defaultValue = $defaultValue;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->sendBy === $other->sendBy
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type)
            && $this->defaultValue === $other->defaultValue;
    }

    public function getSendByString(): string {
        switch ($this->sendBy) {
        case self::SEND_BY_VAL:
            return "0";
        case self::SEND_BY_REF:
            return "1";
        case self::SEND_PREFER_REF:
            return "ZEND_SEND_PREFER_REF";
        }
        throw new Exception("Invalid sendBy value");
    }

    public function hasDefaultValue(): bool {
        return $this->defaultValue !== null && $this->defaultValue !== "UNKNOWN";
    }

    public function getDefaultValueString(): string {
        if ($this->hasDefaultValue()) {
            return '"' . addslashes($this->defaultValue) . '"';
        }

        return "NULL";
    }
}

interface FunctionNameInterface {
    public function getName(): string;
}

interface AliasInterface extends FunctionNameInterface {
    public function getNamespace(): array;
    public function getFullyQualifiedName(): string;
}

interface FunctionInterface extends FunctionNameInterface {
    public function getFullyQualifiedName(): string;
    public function getNumRequiredArgs(): int;

    /**
     * @return ArgInfo[]
     */
    public function getArgs(): array;
    public function getReturnInfo(): ReturnInfo;

    public function equalsApartFromName(FunctionInterface $other): bool;

    public function getArgInfoName(): string;
    public function getDeclarationKey(): string;
    public function getDeclaration():? string;
    public function getFunctionEntry(): string;
}

abstract class AbstractFunctionName implements FunctionNameInterface {
    /** @var string */
    protected $name;

    public function __construct(string $name) {
        $this->name = $name;
    }

    public function getName(): string {
        return $this->name;
    }
}

class FunctionName extends AbstractFunctionName {

}

class MethodName extends AbstractFunctionName {
    /** @var ClassInfo */
    protected $class;

    public function __construct(string $name, ClassInfo $class) {
        parent::__construct($name);
        $this->class = $class;
    }
}

class FunctionAlias extends FunctionName implements AliasInterface {
    /** @var array */
    protected $namespace;

    public function __construct(string $name, array $namespace = [])
    {
        parent::__construct($name);
        $this->namespace = $namespace;
    }

    public function getNamespace(): array
    {
        return $this->namespace;
    }

    public function getFullyQualifiedName(): string {
        $fqn = $this->getNamespace();
        $fqn[] = $this->getName();

        return join('_', $fqn);
    }
}

class MethodAlias extends MethodName implements AliasInterface  {
    /** @var array */
    protected $namespace;
    /** @var string */
    protected $className;

    public function __construct(string $name, string $className, array $namespace = [])
    {
        parent::__construct($name, new ClassInfo($namespace, $className));
        $this->className = $className;
        $this->namespace = $namespace;
    }

    public function getNamespace(): array
    {
        return $this->namespace;
    }

    public function getFullyQualifiedName(): string {
        $fqn = $this->getNamespace();
        // $fqn[] = $this->className;
        $fqn[] = $this->getName();

        return join('_', $fqn);
    }

    /**
     * @deprecated
     * @return string
     */
    public function getFullyQualifiedClassName(): string {
        $ns = $this->getNamespace();
        $ns[] = $this->className;

        return join('_', $ns);
    }
}

class ReturnInfo {
    /** @var bool */
    public $byRef;
    /** @var Type|null */
    public $type;

    public function __construct(bool $byRef, ?Type $type) {
        $this->byRef = $byRef;
        $this->type = $type;
    }

    public function equals(ReturnInfo $other): bool {
        return $this->byRef === $other->byRef
            && Type::equals($this->type, $other->type);
    }
}

abstract class AbstractFunction implements FunctionInterface {
    /** @var FunctionNameInterface */
    protected $name;
    /** @var AliasInterface|null */
    protected $alias;
    /** @var bool */
    protected $isDeprecated;
    /** @var ArgInfo[] */
    protected $args;
    /** @var ReturnInfo */
    protected $return;
    /** @var int */
    protected $numRequiredArgs;
    /** @var string|null */
    public $cond;
    /** @var string[] */
    protected $namespace;

    public function __construct(
        FunctionNameInterface $name,
        ?AliasInterface $alias,
        bool $isDeprecated,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond,
        ?array $namespace
    ) {
        $this->name = $name;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
        $this->namespace = $namespace?? [];
    }

    public function getNamespace()
    {
        return $this->namespace;
    }

    public function getName(): string {
        return $this->name->getName();
    }

    public function getNumRequiredArgs(): int {
        return $this->numRequiredArgs;
    }

    /**
     * @return ArgInfo[]
     */
    public function getArgs(): array {
        return $this->args;
    }

    public function getReturnInfo(): ReturnInfo {
        return $this->return;
    }

    public function equalsApartFromName(FunctionInterface $other): bool {
        if (count($this->args) !== count($other->getArgs())) {
            return false;
        }

        for ($i = 0; $i < count($this->args); $i++) {
            if (!$this->args[$i]->equals($other->getArgs()[$i])) {
                return false;
            }
        }

        return $this->return->equals($other->getReturnInfo())
            && $this->numRequiredArgs === $other->getNumRequiredArgs()
            && $this->cond === $other->cond;
    }

}

class FunctionInfo extends AbstractFunction {

    public function getFullyQualifiedName(): string {

        $fqn = $this->getNamespace();
        $fqn[] = $this->getName();

        return join('_', $fqn);
    }

    public function getArgInfoName(): string {
        return sprintf('arginfo_%s', $this->getFullyQualifiedName());
    }

    public function getDeclarationKey(): string
    {
        if ($this->alias) {
            $name = $this->alias->getFullyQualifiedName();
        } else {
            $name = $this->getFullyQualifiedName();
        }

        return "$name|$this->cond";
    }

    public function getDeclaration(): ?string
    {
        $name = $this->alias? $this->alias->getFullyQualifiedName(): $this->getFullyQualifiedName();
        return sprintf("ZEND_FUNCTION(%s);\n", $name);
    }

    public function getFunctionEntry(): string {

        if ($this->alias) {
            return sprintf(
                "\tZEND_FALIAS(%s, %s, %s)\n",
                $this->getFullyQualifiedName(), $this->alias->getFullyQualifiedName(), $this->getArgInfoName()
            );
        }

        $funcName = ($this->isDeprecated)? 'ZEND_DEP_FE':'ZEND_FE';
        return sprintf("\t%s(%s, %s)\n", $funcName, $this->getFullyQualifiedName(), $this->getArgInfoName());
    }
}

class MethodInfo extends AbstractFunction {
    /** @var int */
    public $flags;
    /** @var ClassInfo */
    protected $classInfo;

    public function __construct(FunctionNameInterface $name, int $flags, ?AliasInterface $alias, bool $isDeprecated, array $args, ReturnInfo $return, int $numRequiredArgs, ?string $cond, ClassInfo $classInfo)
    {
        parent::__construct($name, $alias, $isDeprecated, $args, $return, $numRequiredArgs, $cond, $classInfo->getNamespace());
        $this->flags = $flags;
        $this->classInfo = $classInfo;
    }

    public function getFullyQualifiedClassName(): string {
        $fqn = $this->classInfo->getNamespace();
        $fqn[] = $this->classInfo->getName();

        return join('_', $fqn);
    }

    public function getFullyQualifiedName(): string {

        $fqn = $this->classInfo->getNamespace();
        $fqn[] = $this->classInfo->getName();
        $fqn[] = $this->getName();

        return join('_', $fqn);
    }

    public function getArgInfoName(): string {
        return sprintf('arginfo_class_%s', $this->getFullyQualifiedName());
    }

    public function getDeclarationKey(): string
    {
        if ($this->alias) {
            $name = $this->alias->getFullyQualifiedName();
        } else {
            $name = $this->getFullyQualifiedName();
        }

        return "$name|$this->cond";
    }

    public function getDeclaration(): ?string
    {
        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            return null;
        }

        $name = $this->alias ?? $this->name;

        if ($name instanceof FunctionAlias) {
            return sprintf("ZEND_FUNCTION(%s);\n", $name->getFullyQualifiedName());
        } elseif ($name instanceof MethodAlias) {
            return null;
        }

        return sprintf("ZEND_METHOD(%s, %s);\n", $this->getFullyQualifiedClassName(), $name->getName());
    }

    public function getFunctionEntry(): string {

        if (!$this->alias) {
            $funcName = ($this->flags & Class_::MODIFIER_ABSTRACT)? 'ZEND_ABSTRACT_ME_WITH_FLAGS':'ZEND_ME';

            return sprintf(
                "\t%s(%s, %s, %s, %s)\n",
                $funcName,
                $this->getFullyQualifiedClassName(),
                $this->getName(),
                $this->getArgInfoName(),
                $this->getFlagsAsString()
            );
        }

        if ($this->alias instanceof MethodAlias) {
            return  sprintf(
                "\tZEND_MALIAS(%s, %s, %s, %s, %s)\n",
                $this->alias->getFullyQualifiedClassName(), $this->name->getName(), $this->alias->getFullyQualifiedName(), $this->getArgInfoName(), $this->getFlagsAsString()
            );
        }

        // FunctionAlias
        return sprintf(
            "\tZEND_ME_MAPPING(%s, %s, %s, %s)\n",
            $this->name->getName(), $this->alias->getFullyQualifiedName(), $this->getArgInfoName(), $this->getFlagsAsString()
        );
    }

    private function getFlagsAsString(): string
    {
        $flags = "ZEND_ACC_PUBLIC";
        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $flags = "ZEND_ACC_PROTECTED";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $flags = "ZEND_ACC_PRIVATE";
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $flags .= "|ZEND_ACC_STATIC";
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $flags .= "|ZEND_ACC_FINAL";
        }

        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            $flags .= "|ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $flags .= "|ZEND_ACC_DEPRECATED";
        }

        return $flags;
    }
}

class ClassInfo {
    /** @var string[] */
    protected $namespace;
    /** @var string */
    public $name;
    /** @var MethodInfo[] */
    public $funcInfos;

    public function __construct(?array $namespace, string $name, array $funcInfos = []) {
        $this->namespace = $namespace?? [];
        $this->name = $name;
        $this->funcInfos = $funcInfos;
    }

    public function getNamespace(): array {
        return $this->namespace;
    }

    public function getName(): string {
        return $this->name;
    }

    public function getFullyQualifiedName(): string {
        $fqn = $this->getNamespace();
        $fqn[] = $this->getName();

        return join('_', $fqn);
    }

    public function addFunctionInfo(FunctionInterface $function) {
        $this->funcInfos[] = $function;
    }

    public function getFunctionEntry(): string {
        $code = "";

        $code .= "\n\nstatic const zend_function_entry class_{$this->getFullyQualifiedName()}_methods[] = {\n";
        $code .= generateCodeWithConditions($this->funcInfos, "", function (MethodInfo $funcInfo) {
            return $funcInfo->getFunctionEntry();
        });
        $code .= "\tZEND_FE_END\n";
        $code .= "};\n";

        return $code;
    }
}

class FileInfo {
    /** @var FunctionInterface[] */
    public $funcInfos;
    /** @var ClassInfo[] */
    public $classInfos;
    /** @var bool */
    public $generateFunctionEntries;

    public function __construct(array $funcInfos, array $classInfos, bool $generateFunctionEntries) {
        $this->funcInfos = $funcInfos;
        $this->classInfos = $classInfos;
        $this->generateFunctionEntries = $generateFunctionEntries;
    }

    public function getFunctionEntries(): string {

        $code = "";

        if (!empty($this->funcInfos)) {
            $code .= "\n\nstatic const zend_function_entry ext_functions[] = {\n";
            $code .= generateCodeWithConditions($this->funcInfos, "", function (FunctionInterface $funcInfo) {
                return $funcInfo->getFunctionEntry();
            });
            $code .= "\tZEND_FE_END\n";
            $code .= "};\n";
        }

        foreach ($this->classInfos as $classInfo) {
            $code .= $classInfo->getFunctionEntry();
        }

        return $code;
    }

    /**
     * @return iterable<FunctionInterface>
     */
    public function getAllFuncInfos(): iterable {
        yield from $this->funcInfos;
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->funcInfos;
        }
    }
}

class DocCommentTag {
    /** @var string */
    public $name;
    /** @var string|null */
    public $value;

    public function __construct(string $name, ?string $value) {
        $this->name = $name;
        $this->value = $value;
    }

    public function getValue(): string {
        if ($this->value === null) {
            throw new Exception("@$this->name does not have a value");
        }

        return $this->value;
    }

    public function getVariableName(): string {
        $value = $this->getValue();
        if ($value === null || strlen($value) === 0 || $value[0] !== '$') {
            throw new Exception("@$this->name not followed by variable name");
        }

        return substr($value, 1);
    }
}

/**
 * @param DocComment $comment
 *
 * @return DocCommentTag[]
 */
function parseDocComment(DocComment $comment): array {
    $commentText = substr($comment->getText(), 2, -2);
    $tags = [];
    foreach (explode("\n", $commentText) as $commentLine) {
        $regex = '/^\*\s*@([a-z-]+)(?:\s+(.+))?$/';
        if (preg_match($regex, trim($commentLine), $matches)) {
            $tags[] = new DocCommentTag($matches[1], $matches[2] ?? null);
        }
    }

    return $tags;
}

function parseFunctionLike(
    PrettyPrinterAbstract $prettyPrinter,
    FunctionNameInterface $functionName,
    int $flags,
    Node\FunctionLike $func,
    ?string $cond,
    $namespaceOrClassInfo = null
): FunctionInterface {
    $comment = $func->getDocComment();
    $paramMeta = [];
    $alias = null;
    $isDeprecated = false;
    $haveDocReturnType = false;

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'prefer-ref') {
                $varName = $tag->getVariableName();
                if (!isset($paramMeta[$varName])) {
                    $paramMeta[$varName] = [];
                }
                $paramMeta[$varName]['preferRef'] = true;
            } else if ($tag->name === 'alias') {

                // @alias \Absolute\Another\function
                // @alias \Absolute\Another\Classname::Method
                // @alias Relative\function
                // @alias Relative\Classname::Method
                // @alias ReflectionClass::__clone
                // @alias \ReflectionClass::__clone
                // @alias functionName
                // @alias \globalFunction
                $aliasString = $tag->getValue();

                // cut namespace
                $namespaceParts = explode("\\", $aliasString);
                $name = array_pop($namespaceParts);
                // absolute
                if ($namespaceParts && $namespaceParts[0] === "") {
                    $namespace = array_slice($namespaceParts, 1);
                } else {
                    // relative
                    if ($namespaceOrClassInfo instanceof ClassInfo) {
                        $current_namespace = $namespaceOrClassInfo->getNamespace();
                    } elseif ($namespaceOrClassInfo) {
                        $current_namespace = $namespaceOrClassInfo;
                    } else {
                        $current_namespace = [];
                    }

                    $namespace = array_merge($current_namespace, $namespaceParts);
                }

                $aliasParts = explode("::", $name);
                if (count($aliasParts) === 1) {
                    $alias = new FunctionAlias($aliasParts[0], $namespace);
                } else {
                    $alias = new MethodAlias($aliasParts[1], $aliasParts[0], $namespace);
                }

            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
            } else if ($tag->name === 'return') {
                $haveDocReturnType = true;
            }
        }
    }

    $args = [];
    $numRequiredArgs = 0;
    $foundVariadic = false;
    foreach ($func->getParams() as $i => $param) {
        $varName = $param->var->name;
        $preferRef = !empty($paramMeta[$varName]['preferRef']);
        unset($paramMeta[$varName]);

        if ($preferRef) {
            $sendBy = ArgInfo::SEND_PREFER_REF;
        } else if ($param->byRef) {
            $sendBy = ArgInfo::SEND_BY_REF;
        } else {
            $sendBy = ArgInfo::SEND_BY_VAL;
        }

        if ($foundVariadic) {
            throw new Exception(sprintf("Error in function %s: only the last parameter can be variadic", $functionName->getName()));
        }

        $type = $param->type ? Type::fromNode($param->type) : null;
        if ($param->default instanceof Expr\ConstFetch &&
            $param->default->name->toLowerString() === "null" &&
            $type && !$type->isNullable()
        ) {
            throw new Exception(
                sprintf("Parameter %s of function %s has null default, but is not nullable", $varName, $functionName->getName()));
        }

        $foundVariadic = $param->variadic;

        $args[] = new ArgInfo(
            $varName,
            $sendBy,
            $param->variadic,
            $type,
            $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null
        );
        if (!$param->default && !$param->variadic) {
            $numRequiredArgs = $i + 1;
        }
    }

    foreach (array_keys($paramMeta) as $var) {
        throw new Exception(sprintf("Found metadata for invalid param %s of function %s", $var, $functionName->getName()));
    }

    $returnType = $func->getReturnType();
    if ($returnType === null && !$haveDocReturnType && strpos($functionName->getName(), '__') !== 0) {
        throw new Exception(sprintf("Missing return type for function %s()", $functionName->getName()));
    }

    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null
    );

    // FIXME: For the moment only quick and dirty
    if ($functionName instanceof MethodName) {
        return new MethodInfo(
            $functionName,
            $flags,
            $alias,
            $isDeprecated,
            $args,
            $return,
            $numRequiredArgs,
            $cond,
            $namespaceOrClassInfo
        );
    } else {
        return new FunctionInfo(
            $functionName,
            $alias,
            $isDeprecated,
            $args,
            $return,
            $numRequiredArgs,
            $cond,
            $namespaceOrClassInfo
        );
    }
}

function handlePreprocessorConditions(array &$conds, Stmt $stmt): ?string {
    foreach ($stmt->getComments() as $comment) {
        $text = trim($comment->getText());
        if (preg_match('/^#\s*if\s+(.+)$/', $text, $matches)) {
            $conds[] = $matches[1];
        } else if (preg_match('/^#\s*ifdef\s+(.+)$/', $text, $matches)) {
            $conds[] = "defined($matches[1])";
        } else if (preg_match('/^#\s*ifndef\s+(.+)$/', $text, $matches)) {
            $conds[] = "!defined($matches[1])";
        } else if (preg_match('/^#\s*else$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered else without corresponding #if");
            }
            $cond = array_pop($conds);
            $conds[] = "!($cond)";
        } else if (preg_match('/^#\s*endif$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered #endif without corresponding #if");
            }
            array_pop($conds);
        } else if ($text[0] === '#') {
            throw new Exception("Unrecognized preprocessor directive \"$text\"");
        }
    }

    return empty($conds) ? null : implode(' && ', $conds);
}

function getFileDocComment(array $stmts): ?DocComment {
    if (empty($stmts)) {
        return null;
    }

    $comments = $stmts[0]->getComments();
    if (empty($comments)) {
        return null;
    }

    if ($comments[0] instanceof DocComment) {
        return $comments[0];
    }

    return null;
}

function parseStmts(array $stmts, array $namespace = null) {

    $prettyPrinter = new CustomPrettyPrinter();
    $funcInfos = [];
    $classInfos = [];
    $conds = [];

    foreach ($stmts as $stmt) {

        if ($stmt instanceof Stmt\Namespace_) {
            $namespace = $stmt->name ? $stmt->name->parts : null;
            $result = parseStmts($stmt->stmts, $namespace);
            $funcInfos = array_merge($funcInfos, $result[0]);
            $classInfos = array_merge($classInfos, $result[1]);
            continue;
        }

        $cond = handlePreprocessorConditions($conds, $stmt);
        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Function_) {
            $funcInfos[] = parseFunctionLike(
                $prettyPrinter,
                new FunctionName($stmt->name->toString()),
                0,
                $stmt,
                $cond,
                $namespace
            );

            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $classInfo = new ClassInfo($namespace, $stmt->name->toString());

            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                if (!$classStmt instanceof Stmt\ClassMethod) {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }

                $flags = $classStmt->flags;
                if ($stmt instanceof Stmt\Interface_) {
                    $flags |= Class_::MODIFIER_ABSTRACT;
                }

                if (!($flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                    throw new Exception("Method visibility modifier is required");
                }

                $methodInfo = parseFunctionLike(
                    $prettyPrinter,
                    new MethodName($classStmt->name->toString(), $classInfo),
                    $flags,
                    $classStmt,
                    $cond,
                    $classInfo
                );
                $classInfo->addFunctionInfo($methodInfo);
            }

            $classInfos[] = $classInfo;
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }

    return [$funcInfos, $classInfos];
}

function parseStubFile(string $fileName): FileInfo {
    if (!file_exists($fileName)) {
        throw new Exception("File $fileName does not exist");
    }

    $code = file_get_contents($fileName);

    $lexer = new PhpParser\Lexer();
    $parser = new PhpParser\Parser\Php7($lexer);
    $nodeTraverser = new PhpParser\NodeTraverser;
    $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);

    $stmts = $parser->parse($code);
    $nodeTraverser->traverse($stmts);

    $generateFunctionEntries = false;
    $fileDocComment = getFileDocComment($stmts);
    if ($fileDocComment) {
        if (strpos($fileDocComment->getText(), '@generate-function-entries') !== false) {
            $generateFunctionEntries = true;
        }
    }

    $result = parseStmts($stmts);
    return new FileInfo($result[0], $result[1], $generateFunctionEntries);
}

function funcInfoToCode(FunctionInterface $funcInfo): string {
    $code = '';
    $returnInfo = $funcInfo->getReturnInfo();
    $returnType = $returnInfo->type;
    if ($returnType !== null) {
        if (null !== $simpleReturnType = $returnType->tryToSimpleType()) {
            if ($simpleReturnType->isBuiltin) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $returnInfo->byRef,
                    $funcInfo->getNumRequiredArgs(),
                    $simpleReturnType->toTypeCode(), $returnType->isNullable()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $returnInfo->byRef,
                    $funcInfo->getNumRequiredArgs(),
                    $simpleReturnType->toEscapedName(), $returnType->isNullable()
                );
            }
        } else if (null !== $representableType = $returnType->tryToRepresentableType()) {
            if ($representableType->classType !== null) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(%s, %d, %d, %s, %s)\n",
                    $funcInfo->getArgInfoName(), $returnInfo->byRef,
                    $funcInfo->getNumRequiredArgs(),
                    $representableType->classType->toEscapedName(), $representableType->toTypeMask()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(%s, %d, %d, %s)\n",
                    $funcInfo->getArgInfoName(), $returnInfo->byRef,
                    $funcInfo->getNumRequiredArgs(),
                    $representableType->toTypeMask()
                );
            }
        } else {
            throw new Exception('Unimplemented');
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n",
            $funcInfo->getArgInfoName(), $returnInfo->byRef, $funcInfo->getNumRequiredArgs()
        );
    }

    foreach ($funcInfo->getArgs() as $argInfo) {
        $argKind = $argInfo->isVariadic ? "ARG_VARIADIC" : "ARG";
        $argDefaultKind = $argInfo->hasDefaultValue() ? "_WITH_DEFAULT_VALUE" : "";
        $argType = $argInfo->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable(),
                        $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind,$argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toEscapedName(), $argType->isNullable(),
                        $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
                    );
                }
            } else if (null !== $representableType = $argType->tryToRepresentableType()) {
                if ($representableType->classType !== null) {
                    throw new Exception('Unimplemented');
                }
                $code .= sprintf(
                    "\tZEND_%s_TYPE_MASK(%s, %s, %s, %s)\n",
                    $argKind, $argInfo->getSendByString(), $argInfo->name,
                    $representableType->toTypeMask(),
                    $argInfo->getDefaultValueString()
                );
            } else {
                throw new Exception('Unimplemented');
            }
        } else {
            $code .= sprintf(
                "\tZEND_%s_INFO%s(%s, %s%s)\n",
                $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
            );
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code . "\n";
}

/**
 * @param FunctionInterface[] $generatedFuncInfos
 * @param FunctionInterface   $funcInfo
 *
 * @return FunctionInterface|null
 */
function findEquivalentFuncInfo(array $generatedFuncInfos, FunctionInterface $funcInfo): ?FunctionInterface {
    foreach ($generatedFuncInfos as $generatedFuncInfo) {
        if ($generatedFuncInfo->equalsApartFromName($funcInfo)) {
            return $generatedFuncInfo;
        }
    }
    return null;
}

/**
 * @param iterable<FunctionInterface> $funcInfos
 * @param string                      $separator
 * @param Closure                     $codeGenerator
 *
 * @return string
 */
function generateCodeWithConditions(iterable $funcInfos, string $separator, Closure $codeGenerator): string {
    $code = "";
    foreach ($funcInfos as $funcInfo) {
        $funcCode = $codeGenerator($funcInfo);
        if ($funcCode === null) {
            continue;
        }

        $code .= $separator;
        if ($funcInfo->cond) {
            $code .= "#if {$funcInfo->cond}\n";
            $code .= $funcCode;
            $code .= "#endif\n";
        } else {
            $code .= $funcCode;
        }
    }
    return $code;
}

function generateArgInfoCode(FileInfo $fileInfo): string {
    $code = "/* This is a generated file, edit the .stub.php file instead. */\n";
    $generatedFuncInfos = [];
    $code .= generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        function (FunctionInterface $funcInfo) use(&$generatedFuncInfos) {
            /* If there already is an equivalent arginfo structure, only emit a #define */
            if ($generatedFuncInfo = findEquivalentFuncInfo($generatedFuncInfos, $funcInfo)) {
                $code = sprintf(
                    "#define %s %s\n",
                    $funcInfo->getArgInfoName(), $generatedFuncInfo->getArgInfoName()
                );
            } else {
                $code = funcInfoToCode($funcInfo);
            }

            $generatedFuncInfos[] = $funcInfo;
            return $code;
        }
    );

    if ($fileInfo->generateFunctionEntries) {
        $code .= "\n\n";

        $generatedFunctionDeclarations = [];
        $code .= generateCodeWithConditions(
            $fileInfo->getAllFuncInfos(), "",
            function (FunctionInterface $funcInfo) use(&$generatedFunctionDeclarations) {
                $key = $funcInfo->getDeclarationKey();
                if (isset($generatedFunctionDeclarations[$key])) {
                    return null;
                }

                $generatedFunctionDeclarations[$key] = true;

                return $funcInfo->getDeclaration();
            }
        );

        $code .= $fileInfo->getFunctionEntries();
    }

    return $code;
}

function initPhpParser() {
    $version = "4.3.0";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        $cwd = getcwd();
        chdir(__DIR__);

        passthru("wget https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz", $exit);
        if ($exit !== 0) {
            passthru("curl -LO https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz", $exit);
        }
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP-Parser tarball");
        }
        if (!mkdir($phpParserDir)) {
            throw new Exception("Failed to create directory $phpParserDir");
        }
        passthru("tar xvzf v$version.tar.gz -C PHP-Parser-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract PHP-Parser tarball");
        }
        unlink(__DIR__ . "/v$version.tar.gz");
        chdir($cwd);
    }

    spl_autoload_register(function(string $class) use($phpParserDir) {
        if (strpos($class, "PhpParser\\") === 0) {
            $fileName = $phpParserDir . "/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}

if ($argc >= 2) {
    if (is_file($argv[1])) {
        // Generate single file.
        processStubFile($argv[1]);
    } else if (is_dir($argv[1])) {
        processDirectory($argv[1]);
    } else {
        echo "$argv[1] is neither a file nor a directory.\n";
        exit(1);
    }
} else {
    // Regenerate all stub files we can find.
    processDirectory('.');
}
