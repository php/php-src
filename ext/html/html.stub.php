<?php

/** @generate-class-entries */

namespace Html {

    require "Zend/zend_attributes.stub.php";

    interface Htmlable extends \Stringable
    {
        /**
         * Produces the markup this value renders as: ultimately an
         * Html\Element, Html\Fragment, or Html\Raw, though returning any
         * other Htmlable (e.g. another component instance) is allowed and
         * is resolved recursively at render time.
         */
        public function toHtml(): \Html\Htmlable;

        /**
         * Renders the value to a safely-escaped HTML string. A userland
         * class implementing Htmlable that does not declare (or inherit) a
         * __toString() of its own receives a default implementation - which
         * serializes what toHtml() produces - at class-link time, the way
         * enums receive cases(). Only toHtml() must be written by hand.
         */
        public function __toString(): string;
    }

    /**
     * A single HTML element: a tag, a string-keyed attribute map, and an
     * ordered list of children. Renders to escape-by-default HTML.
     *
     * @strict-properties
     */
    final class Element implements \Html\Htmlable
    {
        public readonly string $tag;
        public readonly array $attributes;
        public readonly array $children;

        public function __construct(string $tag, array $attributes = [], array $children = []) {}

        /** This element itself: the node classes are the base cases of toHtml() resolution. */
        public function toHtml(): \Html\Htmlable {}

        public function __toString(): string {}

        /**
         * Converts this markup to nodes owned by the given modern DOM document,
         * or by a new empty `Dom\HTMLDocument` when none is given.
         * Parsed in "in body" context: top-level context-sensitive elements
         * (`<td>`, `<tr>`, ...) need their enclosing table converted with them.
         */
        public function toDom(?\Dom\Document $document = null): \Dom\DocumentFragment {}
    }

    /**
     * An ordered list of children with no wrapping element. Produced by `<>…</>`.
     *
     * @strict-properties
     */
    final class Fragment implements \Html\Htmlable
    {
        public readonly array $children;

        public function __construct(array $children = []) {}

        /** This fragment itself: the node classes are the base cases of toHtml() resolution. */
        public function toHtml(): \Html\Htmlable {}

        public function __toString(): string {}

        /**
         * Converts this markup to nodes owned by the given modern DOM document,
         * or by a new empty `Dom\HTMLDocument` when none is given.
         * Parsed in "in body" context: top-level context-sensitive elements
         * (`<td>`, `<tr>`, ...) need their enclosing table converted with them.
         */
        public function toDom(?\Dom\Document $document = null): \Dom\DocumentFragment {}
    }

    /**
     * Opaque, already-safe HTML. Passes through serialization un-escaped.
     * Backs both `Html\raw()` and `Html\escape()`.
     *
     * @strict-properties
     */
    final class Raw implements \Html\Htmlable
    {
        public readonly string $html;

        public function __construct(string $html) {}

        /** This raw chunk itself: the node classes are the base cases of toHtml() resolution. */
        public function toHtml(): \Html\Htmlable {}

        public function __toString(): string {}

        /**
         * Converts this markup to nodes owned by the given modern DOM document,
         * or by a new empty `Dom\HTMLDocument` when none is given.
         * Parsed in "in body" context: top-level context-sensitive elements
         * (`<td>`, `<tr>`, ...) need their enclosing table converted with them.
         */
        public function toDom(?\Dom\Document $document = null): \Dom\DocumentFragment {}
    }

    /**
     * Marks a component parameter as a slot. A bare `#[Html\Slot]` is the
     * anonymous (default) slot; `#[Html\Slot('footer')]` is a named slot.
     */
    #[\Attribute(\Attribute::TARGET_PARAMETER)]
    final class Slot
    {
        public readonly ?string $name;

        public function __construct(?string $name = null) {}
    }

    /**
     * Wraps a trusted string as already-safe HTML. The explicit, greppable
     * opt-out of escape-by-default.
     */
    function raw(string $html): \Html\Htmlable {}

    /**
     * Escapes a string and returns it as already-safe HTML, so it passes
     * through later serialization without being escaped a second time.
     */
    function escape(string $text): \Html\Htmlable {}

    /**
     * Returns the plain-PHP source that $code compiles to, with markup
     * expressions lowered to their new \Html\Element(...) /
     * \Html\render_component(...) form. $code is a complete source file
     * including its opening <?php tag. Throws ParseError on invalid code.
     * A debugging and educational aid (see ext/html/markup2php); the output
     * is equivalent source, not a supported compilation target.
     */
    function transpile(string $code): string {}

    /**
     * Dispatches a component (RFC §3 and §5) and returns the Html\Htmlable it
     * produces. This is the runtime target a `<Component …/>` markup tag lowers
     * into; it is exposed as a public function so the dispatch and slot-routing
     * rules are directly testable, but user code is expected to write tags.
     *
     * A bare `<Component/>` tag could name either a class or a function, which PHP
     * resolves through separate import tables (`use` vs `use function`). The
     * compiler therefore passes two already-resolved candidates: $component (the
     * class-resolved name) and $functionComponent (the function-resolved name; a
     * list of names — current namespace first, then global — when an unqualified
     * name in a namespace gets PHP's usual global function fallback, tried in
     * order). They are resolved in this order:
     *   1. "Class::method" in $component - a static-method component (explicit);
     *   2. $component names a class implementing Html\Htmlable - instantiated;
     *   3. $functionComponent (or $component, when $functionComponent is null)
     *      names a userland function - called; its result must be Html\Htmlable;
     *   4. otherwise (an internal/builtin function, or no such symbol) a hard
     *      error - this is the `<Date/>` → `date()` footgun guard.
     *
     * When called directly with a single name, leave $functionComponent null: the
     * one name is then tried as both a class and a function.
     *
     * $props become named arguments. The anonymous body $slot is routed to the
     * parameter marked with a bare #[Html\Slot]; each entry of $namedSlots is
     * routed to the parameter marked #[Html\Slot(name)]. A parameter filled by
     * both a prop and a slot, a slot with no matching parameter, more than one
     * anonymous slot, or duplicate slot names are all errors.
     *
     * @param array<string, mixed> $props
     * @param array<string, \Html\Htmlable> $namedSlots
     */
    function render_component(
        string $component,
        array $props = [],
        ?\Html\Htmlable $slot = null,
        array $namedSlots = [],
        array|string|null $functionComponent = null,
    ): \Html\Htmlable {}

    /**
     * Renders a dynamic tag (RFC §4): the runtime target a `<$tag …>…</$tag>`
     * or `<{expr} …>…</>` markup tag lowers into. $tag's value decides at runtime exactly what a
     * static tag name decides at compile time, by the same classification
     * rule: a name with an uppercase first character, a "\", or a "::"
     * dispatches as a component (so `$tag = Card::class` behaves like
     * `<Card/>`, hooks and decorators included, with $attributes as props,
     * loose $children as the anonymous slot, and $namedSlots routed to
     * #[Html\Slot(name)] parameters); anything else constructs a literal
     * `new \Html\Element($tag, $attributes, $children)` (so `$tag = 'div'`
     * behaves like `<div>`). Like a direct render_component() call, a dynamic
     * component name is tried as a class and then as a function; there is no
     * compile-time `use` resolution, so pass a fully-qualified name
     * (Foo::class). $namedSlots with an element-classified $tag is an error.
     *
     * @param array<string, mixed> $attributes
     * @param array<int, mixed> $children
     * @param array<string, \Html\Htmlable> $namedSlots
     */
    function render_dynamic(
        string $tag,
        array $attributes = [],
        array $children = [],
        array $namedSlots = [],
    ): \Html\Htmlable {}

    /**
     * Register a factory that instantiates *class* components (`<Card/>`), so a
     * framework can resolve them through its container instead of a bare `new`.
     * The engine still resolves the name, confirms the class implements
     * Html\Htmlable, and routes props + #[Html\Slot] content into named arguments;
     * the factory only replaces construction.
     *
     * The factory is called as `$factory(string $class, array $args): ?object`
     * with the resolved class name and the routed named-argument array (the same
     * array the constructor would receive). It must return either an instance of
     * $class (which the engine then uses) or null to defer to the next registered
     * factory (and finally the engine's own `new`). Factories are tried in
     * registration order. Registration is request-scoped.
     *
     * Passing $component (a class FQCN; case-insensitive, with or without a
     * leading backslash) scopes the factory to that one component: it only runs
     * when the resolved class name matches, and every other class component
     * skips it entirely — no userland call is made. Scoped and unscoped
     * factories share one chain, still tried in registration order.
     *
     * Example (Laravel):
     *   Html\register_component_factory(
     *       fn(string $class, array $args) => app()->make($class, $args)
     *   );
     *
     * Example (one component only):
     *   Html\register_component_factory(
     *       fn(string $class, array $args) => new Card(...$args, theme: $theme),
     *       Card::class
     *   );
     */
    function register_component_factory(callable $factory, ?string $component = null): void {}

    /**
     * Remove a previously registered component factory. Returns whether one
     * matched. The registration to remove is identified by both the callable
     * and the $component scope it was registered with (null only matches an
     * unscoped registration).
     */
    function unregister_component_factory(callable $factory, ?string $component = null): bool {}

    /**
     * Register an invoker for *function* and *static-method* components
     * (`<greeting/>`, `<Author::byline/>`), so a framework can call them through
     * its container and autowire parameters the props don't supply. The engine
     * still routes props + slots into named arguments.
     *
     * The invoker is called as `$invoker(callable $component, array $args): ?Html\Htmlable`
     * with the component callable (a function name, or "Class::method") and the
     * routed named-argument array. It must return the produced Html\Htmlable, or
     * null to defer to the next invoker (and finally the engine's own call).
     * Invokers are tried in registration order. Registration is request-scoped.
     *
     * Passing $component scopes the invoker to that one component: it only runs
     * when the resolved callable name — the same name the invoker would receive,
     * i.e. the function's FQCN or "Class::method" — matches (case-insensitive,
     * with or without a leading backslash). Every other component skips it
     * entirely, with no userland call.
     *
     * Example (Laravel):
     *   Html\register_component_invoker(
     *       fn(callable $fn, array $args) => app()->call($fn, $args)
     *   );
     */
    function register_component_invoker(callable $invoker, ?string $component = null): void {}

    /**
     * Remove a previously registered component invoker. Returns whether one
     * matched. The registration to remove is identified by both the callable
     * and the $component scope it was registered with (null only matches an
     * unscoped registration).
     */
    function unregister_component_invoker(callable $invoker, ?string $component = null): bool {}

    /**
     * Register a decorator that runs on the Html\Htmlable every component produces
     * — class, function, and static-method alike — after it has been constructed
     * or called. This is the cross-cutting seam (as opposed to the kind-specific
     * factory/invoker production seams): use it to wrap, transform, log, profile,
     * or cache component output uniformly.
     *
     * The decorator is called as
     * `$decorator(Html\Htmlable $rendered, string $component): Html\Htmlable`
     * with the produced value and the resolved component name, and must return an
     * Html\Htmlable. Decorators compose in registration order, each receiving the
     * previous one's result. Registration is request-scoped.
     *
     * Passing $component scopes the decorator to that one component: it only
     * runs when the resolved component name — the same name the decorator
     * receives — matches (case-insensitive, with or without a leading
     * backslash). Every other component skips it entirely.
     *
     * Example: uppercase every component's output.
     *   Html\register_component_decorator(
     *       fn(Html\Htmlable $h, string $c) => Html\raw(strtoupper((string) $h))
     *   );
     */
    function register_component_decorator(callable $decorator, ?string $component = null): void {}

    /**
     * Remove a previously registered component decorator. Returns whether one
     * matched. The registration to remove is identified by both the callable
     * and the $component scope it was registered with (null only matches an
     * unscoped registration).
     */
    function unregister_component_decorator(callable $decorator, ?string $component = null): bool {}
}
