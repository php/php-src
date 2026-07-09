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
    }

    /**
     * Marks the component parameter that receives the component's body content
     * (the slot). At most one parameter per component may carry it. Content for
     * additional "slots" is passed as ordinary props whose type is Html\Htmlable
     * (e.g. `<Layout header={<h1>Title</h1>}>…</Layout>`).
     */
    #[\Attribute(\Attribute::TARGET_PARAMETER)]
    final class Slot {}

    /**
     * A slot body whose evaluation is deferred: it builds the child subtree -
     * and runs any side effects in its interpolations - only the first time it
     * is rendered, memoizing the result. Produced by the `:lazy` directive on a
     * component tag (`<Auth :lazy>…</Auth>`), so a component that discards its
     * body never evaluates it. Because it is itself an Html\Htmlable, a
     * component's slot parameter type is unchanged (`?Html\Htmlable`); laziness
     * is transparent to the component.
     *
     * @strict-properties
     */
    final class LazyFragment implements \Html\Htmlable
    {
        /** The thunk builds the deferred body and is called at most once. */
        public function __construct(\Closure $thunk) {}

        /** Evaluates the thunk on first call (memoized) and returns its Html\Htmlable. */
        public function toHtml(): \Html\Htmlable {}

        public function __toString(): string {}
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
     * Dispatches a component and returns the Html\Htmlable it
     * produces. This is the runtime target a `<Component …/>` markup tag lowers
     * into; it is exposed as a public function so the dispatch and slot-routing
     * rules are directly testable, but user code is expected to write tags.
     *
     * A component is a class implementing Html\Htmlable, or a public static
     * method on a class ("Class::method"). $component is the already-resolved
     * name (the compiler resolves the tag - the class part of a "::" tag
     * included - against `use` imports and the current namespace, as
     * `Component::class` does). A class name is looked up, confirmed to
     * implement Html\Htmlable, and instantiated; a "Class::method" name is
     * called and must return an Html\Htmlable. Anything else - no such class,
     * a class not implementing the interface, a non-public/non-static method -
     * is a hard error. Plain functions are not components (Future Scope).
     *
     * $props become named arguments. The body $slot is routed to the parameter
     * marked with #[Html\Slot]. A parameter filled by both a prop and the body,
     * more than one #[Html\Slot] parameter, or body content with no #[Html\Slot]
     * parameter are all errors.
     *
     * @param array<string, mixed> $props
     */
    function render_component(
        string $component,
        array $props = [],
        ?\Html\Htmlable $slot = null,
    ): \Html\Htmlable {}

    /**
     * Renders a dynamic tag: the runtime target a `<$tag …>…</$tag>`
     * or `<{expr} …>…</>` markup tag lowers into. $tag's value decides at runtime exactly what a
     * static tag name decides at compile time, by the same classification
     * rule: a name with an uppercase first character, a "\", or a "::"
     * dispatches as a component (so `$tag = Card::class` behaves like
     * `<Card/>`, hooks and decorators included, with $attributes as props and
     * $children as the body slot); anything else constructs a literal
     * `new \Html\Element($tag, $attributes, $children)` (so `$tag = 'div'`
     * behaves like `<div>`). There is no compile-time `use` resolution, so pass
     * a fully-qualified name (Foo::class).
     *
     * @param array<string, mixed> $attributes
     * @param array<int, mixed> $children
     */
    function render_dynamic(
        string $tag,
        array $attributes = [],
        array $children = [],
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
     * skips it entirely - no userland call is made. Scoped and unscoped
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
     * Register a decorator that runs on the Html\Htmlable every component
     * produces - class and static-method alike - after it has been constructed
     * or called. This is the cross-cutting seam (as opposed to the factory
     * production seam): use it to wrap, transform, log, profile, or cache
     * component output uniformly.
     *
     * The decorator is called as
     * `$decorator(Html\Htmlable $rendered, string $component): Html\Htmlable`
     * with the produced value and the resolved component name, and must return an
     * Html\Htmlable. Decorators compose in registration order, each receiving the
     * previous one's result. Registration is request-scoped.
     *
     * Passing $component scopes the decorator to that one component: it only
     * runs when the resolved component name - the same name the decorator
     * receives - matches (case-insensitive, with or without a leading
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
