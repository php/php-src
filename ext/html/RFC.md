# PHP RFC: Native Markup Expressions (JSX-style syntax)

## Introduction

PHP began as a templating language - code embedded *inside* HTML. As the language and its ecosystem have matured, the inverse has become just as desirable: embedding HTML *inside* PHP, as a first-class value you can compose, return, and pass around.

Yet PHP still has no native way to do it. Today authors choose between:

* **Inline HTML** (`?> <h1><?= $x ?></h1> <?php`) - output-only: it cannot be captured as a value without output buffering, offers no composition, does not escape, and the tag-soup syntax is verbose and hard to read.
* **String concatenation / heredocs** - no escaping, no structure, error-prone.
* **Userland template engines** (Blade, Twig, Latte) - powerful, but each requires a separate template dialect and files, a compilation step, a parser written in PHP, and its own third-party IDE/tooling support.

This RFC proposes a **native markup expression syntax**, inspired by JSX, that produces an in-memory tree of objects which renders to safely-escaped HTML. Markup becomes a *value*: it can be returned, passed to functions, stored, and composed.

```php
function Greeting(string $name, string $type): Html\Htmlable
{
    return <>
        <h1 class="title">Hello, {$name ?: ucfirst($type)}!</h1>
        <p>Welcome to PHP, where markup is a first-class expression.</p>
    </>;
}

echo <Greeting name="Rasmus" type="guest" />; // prints the rendered HTML, dynamic values escaped
```

Despite appearances, this is not a new template language grafted onto PHP - the syntax is **pure compile-time sugar**. Every markup expression lowers, during compilation, to a plain `new` expression:

```php
// The new syntax...
$html = <button class="btn">Sign in</button>;

// ...compiles to exactly this - same AST, same opcodes:
$html = new \Html\Element('button', ['class' => 'btn'], ['Sign in']);
```

Nothing downstream of the parser changes, and markup obeys the expression semantics every PHP developer already knows.

The two headline benefits over every existing option are **composition** (components, attributes, slots) and **escape-by-default safety** - neither of which PHP's existing markup facilities provide. And because markup is ordinary PHP code rather than a string dialect, static analysis reaches inside it: component props are named arguments checked against real signatures, so templates become type-checkable end-to-end.

### Why this is worth doing

Generating HTML is not a niche PHP task - it is arguably *the* PHP task, and essentially every major framework has independently built the same answer to it: an escape-by-default, composable template engine. Their adoption is enormous - on Packagist, `laravel/framework` (Blade) has over **540 million** installs and `twig/twig` over **460 million**, before counting Latte, Smarty, Plates, or hand-rolled templates that never appear in a dependency graph. 

That every engine converged on the same two features is strong evidence the need is real and the language does not meet it: developers reach for a third-party compiler, a separate dialect, and a build step to obtain what the runtime could provide natively. And escape-by-default is no mere convenience - cross-site scripting has sat in the OWASP Top 10 for its entire existence, and automatic HTML escaping is one of the disciplines that prevents it.

## History

This idea is not new - PHP is where it started. Around 2010, Facebook built **XHP**, a PHP extension that made XML/HTML fragments valid PHP expressions with automatic escaping - `<a href={$url}>Link</a>` as a real value - and it survives today as a native feature of Hack. **JSX was born directly out of XHP**, and has since become *the* way most frontend developers write user interfaces. The pattern has proven itself at ecosystem scale; this RFC brings it home.

The approach has surfaced in various forms many times over the years without a formal proposal; discussions about "automatic template escaping" resulted in arguments that "XHP really is the right solution for this problem," though that also raised the central objection - escaping is context-dependent (HTML vs URL vs JS vs CSS) - which this RFC answers by scoping escaping to HTML context only (see Escape-by-default).

### Why core, and not an extension

An obvious question - especially given the XHP precedent - is whether this could be proven out as a PECL extension first. It cannot, for one technical reason and one practical one.

Technically, markup must begin at a bare `<` in operand position, and telling that apart from the comparison and shift operators requires the scanner change described under Parsing - state no extension can reach. The only route open to an extension is rewriting source text before compilation (the original XHP extension's approach), which shifts line numbers in errors and stack traces and leaves the raw file something `token_get_all()` - and therefore every tool built on it - cannot tokenize.

Practically, a *syntax* lives or dies by its ecosystem. Code using an extension-only syntax cannot be reasonably published to Packagist - it is a parse error on any install without the extension - and no IDE, formatter, or static analyser updates its grammar for syntax that may not exist on a given machine. That chicken-and-egg is part of where XHP for PHP stalled, while the same feature thrived in Hack, where it is part of the language. Shipping in core is what makes markup part of PHP itself: parsers, IDEs, and analysis tools implement it once, and every developer's markup gets the same highlighting, formatting, and static analysis their ordinary code already enjoys.

## Goals & Non-Goals

The goal of this RFC is a native, ergonomic syntax for building HTML - the output format the overwhelming majority of PHP produces - with escaping and composition built in.

This RFC is deliberately narrower than XHP: it targets **HTML specifically**, not general XML. That choice is visible throughout - escaping uses `htmlspecialchars`; void elements serialize as HTML5 (`<br>`, not `<br/>`); text and attribute handling follow HTML, not XML, semantics. There is no XML namespace support, no CDATA, no DTD, and no requirement that output be well-formed XML (`<slot:footer>` *looks* like a namespaced XML element but is a compile-time construct, not a namespace). This keeps the surface small and the output aimed at the one thing PHP overwhelmingly produces: web pages. Emitting XML (RSS, SVG, SOAP) remains the job of the existing DOM / `XMLWriter` APIs and is out of scope.

Semantic validation of attribute *values* is also not a goal - a `javascript:` URL in `href`/`src` escapes cleanly yet is not blocked: sometimes it is intentional, and the policy is the application's to set. (Latte nullifies such URLs and React warns; a framework can enforce the same here via a component decorator or userland wrapper.) Attribute and tag *names*, by contrast, are always validated - see Escape-by-default.

The aim, in short: enough syntax to make HTML a first-class concern of PHP, with the extension points for frameworks to customize behaviour, and room to grow without breaking backward compatibility (see Future Scope).

## Proposal

### 1. A markup expression that evaluates to an object

The core proposal is that a markup expression evaluates to an instance of a built-in type implementing `Html\Htmlable`. It is **not** a string - but casting it to one will return a string value.

This is the JSX model (elements are objects, strings appear only after rendering) and is what makes composition and a single, central escaping step possible.

#### Desugaring at a glance

Every construct in this proposal lowers the same way - at compile time, to a `new` expression or a function call, behaviour PHP already supports well; no other part of the engine changes.

```php
// Attributes: literal, ={expr}, bare boolean, {...spread}     (see Attributes)
<a href={$url} class="btn">{$label}</a>
// → new \Html\Element('a', ['href' => $url, 'class' => 'btn'], [$label])

// Fragments group children with no wrapper element            (see Syntax structure)
<><h1>{$title}</h1><input {...$attrs} required /></>
// → new \Html\Fragment([
//       new \Html\Element('h1', [], [$title]),
//       new \Html\Element('input', [...$attrs, 'required' => true], []),
//   ])

// Components dispatch through Html\render_component()         (see Tags)
<Card title="Hi" />
// → \Html\render_component(Card::class, ['title' => 'Hi'], null, [], 'Card')

// Component body → anonymous slot; <slot:x> blocks → named slots   (see Children & slots)
<Layout theme="dark">
    <slot:header><h1>Title</h1></slot:header>
    <p>Loose content.</p>
</Layout>
// → \Html\render_component(
//       Layout::class,                          // class-name candidate
//       ['theme' => 'dark'],                    // props → named arguments
//       new \Html\Fragment([/* <p>...</p> */]),   // anonymous slot
//       ['header' => new \Html\Fragment([/* <h1>...</h1> */])],  // named slots
//       'Layout',                               // function-name candidate
//   )

// Dynamic tags: the value classifies at runtime                (see Tags)
<$tag class="box">{$content}</$tag>
// → \Html\render_dynamic($tag, ['class' => 'box'], [$content], [])
```

Two properties fall directly out of this lowering. Markup is **zero-cost sugar** - writing the objects by hand produces the same AST and opcodes. And markup obeys **ordinary expression semantics** - attributes and children are argument expressions, evaluated eagerly, scoped and typed like any other PHP code.

### 2. Escape-by-default

This is a *templating* feature, and like every major PHP template engine (Blade, Twig, Latte) it **escapes by default**:

* Interpolations `{$expr}` and **attribute values** are escaped with `htmlspecialchars($v, ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401)` - like `htmlspecialchars()` itself, the charset comes from ini `default_charset` (UTF-8 by default), so non-UTF-8 applications keep working.
* Any value implementing `Html\Htmlable` (including elements and fragments) passes through **un-escaped** - it is already assumed to be safe HTML.
* The explicit, greppable opt-out for trusted raw strings is `Html\raw($trustedString)`, which returns an `Html\Htmlable`. `Html\escape($string)` is also provided.

Escaping is **HTML-context only**. URL/JS/CSS-context escaping is explicitly **out of scope** and remains the author's responsibility. Full context-aware escaping is noted as Future Scope.

A single escaping context is sound here in a way it is not for string-template engines, because there is no context to *detect*:

* The grammar admits dynamic data in few positions - child content and attribute values - each escaped by one fixed rule (attribute values are always serialized double-quoted; no unquoted output form exists).
* Dynamically-supplied *names* (spread keys, `new Element($tag)`) are **validated** and can throw a runtime exception, not escaped - an invalid name is a `ValueError`, so a name can never break out of the markup.
* `<script>`/`<style>` get no raw-text mode: interpolated content there is HTML-escaped like any other child, so `</script>` breakout is inexpressible.

### 3. Tags: HTML elements vs components

Dispatch is purely **syntactic** (the compiler cannot know runtime types):

| Tag form | Meaning |
|---|---|
| `<div>`, `<my-widget>` (lowercase / hyphenated) | literal HTML element → `Html\Element` |
| `<Card>` (bare, capitalized) | a **component** - resolved as a name honoring `use` / `use function` / namespace |
| `<App\Card>`, `<\App\Card>` (namespace-qualified) | a **component** - a namespace-qualified or fully-qualified name |
| `<Author::byline>` | a **component** - a static method call on a class |
| `<$tag>`, `<{expr}>` (a variable / any expression) | a **dynamic tag** - the runtime string value is classified by the same rule, at runtime |

Any tag whose name is capitalized, contains a namespace separator (`\`), or names a static method (`::`) is a component; everything else is a literal HTML element.

A **component is anything that produces `Html\Htmlable`.** There are three forms:

```php
// Function
function Time(DateTimeInterface $datetime): Html\Htmlable { /* ... */ }
// <Time datetime={$datetime} />  →  Time(datetime: $datetime)

// Static method - multiple components can live together on one class
class Author {
    public static function byline(string $name): Html\Htmlable { /* ... */ }
    public static function avatar(string $src): Html\Htmlable { /* ... */ }
}
// <Author::byline name="Rasmus" />  →  Author::byline(name: 'Rasmus')

// Class implementing Html\Htmlable - the instance IS the renderable
class Card implements Html\Htmlable {
    public function __construct(public string $title) {}
    public function toHtml(): Html\Htmlable {
        return <div class="card">{$this->title}</div>;
    }
}
// <Card title="Hi" />  →  new Card(title: 'Hi')
```

`toHtml()` returns markup, not a string, so it composes under `strict_types` with no cast, and callers can still reach the produced `Html\Element` tree. It may also return another `Htmlable` - e.g. delegate to another component instance.

#### Bare-tag and qualified-tag resolution

A component tag names a class or a function. PHP resolves class names and function names through **separate** rules and `use` tables, and a bare `<Foo/>` tag gives no syntactic signal which is meant, so the compiler resolves the name **both ways** and lets the runtime pick:

* as a **class** name - honoring `use` and the current namespace (as `Foo::class` does);
* as a **function** name - honoring `use function` and the current namespace.

The two candidates are then dispatched at runtime in this order:

1. the class candidate names a class implementing `Html\Htmlable` → instantiate it;
2. else the function candidate names a userland function → call it (its result must be `Html\Htmlable`);
3. else → a clear error.

The consequence is that components follow ordinary PHP scoping exactly - a class component is imported with `use`, a function component with `use function`, and using the wrong keyword fails just as it would in a normal `new`/call:

```php
namespace Views;

use App\Card;               // a class component
use function App\Greeting;  // a function component

<Card title="Hi" />   // → new App\Card(title: 'Hi')
<Greeting />          // → App\greeting()
```

Tags may also be written **namespace-qualified** or **fully-qualified** directly; any tag containing `\` is a component regardless of the first letter's case:

```php
<App\Card title="Hi"/>    // namespace-relative
<\App\Card title="Hi"/>   // fully-qualified
```

#### Dynamic tags - `<$tag>` and `<{expr}>`

The tag position also accepts a dynamic value, for the common CMS/builder case where the element or component is data ("render this list of blocks"). It follows the rule PHP developers already know from double-quoted string interpolation: a simple variable may appear bare, and braces admit any expression:

```php
$tag = $isImportant ? 'strong' : 'span';
echo <$tag class="note">{$text}</$tag>;

echo <{$block->type} {...$block->attributes} />;
echo <{$isImportant ? 'strong' : 'span'}>{$text}</>;
```

This is consistent with the rest of the grammar, where braces already mark every dynamic value.

The value is classified **at runtime by exactly the compile-time rule**: a string with an uppercase first character, a `\`, or a `::` dispatches as a component; anything else constructs a literal `Html\Element`. So `$tag = 'div'` behaves like `<div>` and `$component = Card::class` behaves like `<Card/>`, and only classification moves to runtime. Two differences follow from the name being data rather than source: it is not resolved against `use` imports (there is no compile-time name to resolve - pass a fully-qualified name, which `::class` already produces), and element names are validated when the tree is serialized rather than when it is parsed (the existing `Html\Element` rule; an invalid name throws, it can never break out of the markup).

Closing follows the form:
- A variable tag closes by repeating the same variable such as `<$a>...</$a>`
    - `<$a>...</$b>` is a compile error, exactly as `<div>...</span>` is 
- An expression tag with a brace closes **anonymously** with `</>`
    - A closing tag cannot restate the expression - re-evaluating it would run its side effects twice, and matching it textually is meaningless for a computed value; the expression is evaluated exactly once.
- A self-closing `<$tag/>` / `<{expr}/>` needs no closer at all.

#### Userland integration: dispatch hooks

By default the engine constructs a class component with `new` and calls a function or static-method component directly. In userland, particularly within frameworks, they will often want to intervene - to resolve components through a dependency-injection container (autowiring the constructor/parameter dependencies the props do not supply), or to apply a cross-cutting transform to every component's output. Three request-scoped hook lists make this possible without touching any component's code. Each behaves like `spl_autoload_register` - handlers are kept in registration order, with a matching `unregister_*` that reports whether a handler matched:

| Hook | Fires for | Signature | Purpose |
|---|---|---|---|
| `Html\register_component_factory` | class components | `(string $class, array $args): ?object` | replace `new` (e.g. container construction) |
| `Html\register_component_invoker` | function & static-method components | `(callable $component, array $args): ?Html\Htmlable` | replace the call (e.g. parameter autowiring) |
| `Html\register_component_decorator` | **every** component kind | `(Html\Htmlable $rendered, string $component): Html\Htmlable` | transform / wrap / log / cache the output |

The engine always does the part only it can: resolve the name, confirm the target, and route props + `#[Html\Slot]` content into `$args`. A **factory** or **invoker** only changes *how the value is produced* - it returns the value, or `null` to defer to the next handler (and finally to the engine's own `new`/call); a factory's result must be an instance of the requested class, an invoker's must be `Html\Htmlable`.

Construction and invocation are separate hooks because a container needs different information to autowire each (the class name vs. the callable). A **decorator** is the cross-cutting seam: it runs on the `Html\Htmlable` every component produces, composing in registration order. (Plain HTML elements such as `<div>` are not components; no hook fires for them.)

Every `register_*`/`unregister_*` function also takes an optional second argument, `?string $component = null`, that scopes the hook to a single component: the hook only fires when the resolved name it would receive - the class FQCN, the function name, or `"Class::method"` - matches (case-insensitively, with or without a leading backslash). Scoped and unscoped hooks share one chain in registration order, and the dispatch skips out-of-scope hooks inside the engine, before any userland call, so a hook registered for one component costs the others nothing. Unregistering matches on both the callable and the scope it was registered with.

Registration is scoped to the current runtime, so a framework registers hooks during bootstrap; when nothing is registered the hooks add no cost - the default `new`/call path is unchanged.

```php
$container = ...;

// A framework's bootstrap: wire components to the service container...
Html\register_component_factory(
    fn(string $class, array $args) => $container->make($class, $args)
);
Html\register_component_invoker(
    fn(callable $fn, array $args) => $container->call($fn, $args)
);

// ...and a cross-cutting decorator (profiling, caching, wrapping, ...).
Html\register_component_decorator(function (Html\Htmlable $rendered, string $component) {
    return $rendered;
});
```

A component can then simply declare its dependencies; the container supplies them while markup attributes still fill the remaining parameters as named arguments:

```php
final class UserCard implements Html\Htmlable {
    public function __construct(private Clock $clock, public string $name) {}
    public function toHtml(): Html\Htmlable { /* ...uses $this->clock... */ }
}

<UserCard name="Rasmus"/>   // the container injects $clock; the prop fills $name
```

### 4. Attributes

```php
<a href={$url} class="btn" data-id={$id} disabled {...$extra}>{$label}</a>
```

| Form | Meaning |
|---|---|
| `class="btn"` | literal string |
| `href={$expr}` | PHP expression (same `{}` as interpolation) |
| `disabled` (bare) | boolean `true` |
| `{...$attrs}` | spread (array → attributes; named-argument unpacking for components) |

* Attribute names are **native HTML names** (`class`, `for`, `data-x`) - *not* `className`. PHP accepts reserved words as parameter names and named-argument labels, so no `className`/`htmlFor` aliasing is needed like JSX.
* On **HTML elements**, attributes become a string-keyed map.
* On **components**, attributes become **named arguments**. Unknown attributes are a `TypeError` (`Unknown named parameter`) unless the component declares a `...$attrs` variadic, which collects them as a string-keyed array - exactly PHP's existing named-argument + variadic semantics, no new machinery. Hyphenated names (`data-x`, `aria-y`) are not legal named-argument *labels* in a normal call, but as markup attributes they pass through the same mechanism and are collected by the variadic.

#### Attribute value coercion

| Value | Result |
|---|---|
| `null` / `false` | attribute **omitted** |
| `true` | boolean attribute (bare name) |
| `string` / `int` / `float` / `Stringable` | `="escaped value"` |
| `array` / other | `TypeError` |

### 5. Children, slots, and named slots

#### HTML elements

Children are an ordered list of child nodes (text, interpolations, sub-elements).

#### Components - slots via `#[Html\Slot]`

The body of a component is routed to a constructor/function/method parameter marked with the `#[Html\Slot]` attribute.

```php
function Time(
    DateTimeInterface $datetime,
    #[Html\Slot] ?Html\Htmlable $slot = null,
): Html\Htmlable {
    return <time datetime={$datetime->format('c')}>
        {$slot ?? Html\escape($datetime->format('F j'))}
    </time>;
}

// <Time datetime={$d} />            →  Time(datetime: $d)
// <Time datetime={$d}>July 4</Time> →  Time(datetime: $d, slot: <fragment "July 4">)
```

The slot value is always a single `Html\Fragment` (an `Html\Htmlable` whose children are each normalized to `Html\Htmlable`); `{$slot}` renders the whole body, correctly escaped. The fragment's children are introspectable as an opt-in (`$slot->children`).

#### Named slots - `<slot:name>`

`#[Html\Slot]` takes an optional name. Bare `#[Html\Slot]` is the **default/anonymous** slot (receiving loose body content); `#[Html\Slot('footer')]` is a **named** slot, filled by a `<slot:footer>...</slot:footer>` block.

```php
function Layout(
    #[Html\Slot]            Html\Htmlable $body,
    #[Html\Slot('header')] ?Html\Htmlable $header = null,
    #[Html\Slot('footer')] ?Html\Htmlable $footer = null,
): Html\Htmlable { /* ... */ }

<Layout>
    <slot:header><h1>Title</h1></slot:header>
    <p>Loose content flows to the anonymous slot.</p>
    <slot:footer>© 2026</slot:footer>
</Layout>
```

This is also this proposal's answer to **template inheritance**: what Twig, Latte and Blade's older syntax call a base template with overridable *blocks* is here a `Layout` component with named slots - a page fills the slots instead of extending the template, and layouts nest by composition.

Rules for slots are as follows:

* Multiple `#[Html\Slot]` parameters are allowed only with **distinct names** and **at most one anonymous** slot; otherwise an **error**.
* A `<slot:foo>` block with no matching `#[Html\Slot('foo')]` parameter → **error**.
* Filling the same parameter both by attribute and by `<slot:>` block → **error**.
* A `<slot:>` block is only allowed **directly in a component body** (not inside a plain element or fragment) and takes **no attributes** → **compile error** otherwise.
* `#[Html\Slot]` on a variadic parameter → **error** (a slot is a single value).
* Slots are ordinary parameters, so a **required** slot parameter (no default) left unfilled fails at runtime like any missing argument (`ArgumentCountError`). Declare a default (`?Html\Htmlable $footer = null`) to make a slot optional.

#### Eager evaluation

Because a component invocation is an ordinary call, **children are fully evaluated before the parent runs** (PHP evaluates arguments before the call). Consequently a component **cannot conditionally skip or defer rendering its children** out-of-the-box - regular control flow or closures can be used instead. Lazy children (via closures) are Future Scope.

#### Child coercion

| Child value | Renders as |
|---|---|
| `Html\Htmlable` | itself, **not** escaped (resolved through `toHtml()` and serialized natively) |
| `string` / `int` / `float` / `bool` / `null` | cast to string and escaped - so `false`/`null` → `""`, `true` → `"1"` |
| `array` / `Traversable` | flattened, each element coerced recursively |
| non-`Stringable` object / resource | `TypeError` |

This makes control flow ride entirely on `{...}` with no new directives, exactly what JSX has landed on:

```php
<ul>{array_map(fn($i) => <li>{$i->name}</li>, $items)}</ul>      // loop (array flattens)
<div>{$user !== null ? <span>{$user->name}</span> : null}</div>  // conditional (null → "")
<div>{$loggedIn ? <a href="/logout">Log out</a> : null}</div>    // optional (null → "")
```

Conditionals use a ternary, not the JSX `{condition && <x/>}` idiom: in PHP, `&&` evaluates to a boolean, so `{$cond && <x/>}` would render `"1"`, not the element.

### 6. Syntax structure

* **Closing tags must match** their opener, including `</Author::byline>` and the dynamic `</$tag>`. Mismatches are a compile error (catches the most common markup typo early).
* **Childless elements**: `<div></div>` and `<div/>` are equivalent - existing HTML can be pasted into markup unchanged. The parser needs **no hardcoded HTML void-element list**; the serializer still *emits* `<br>` (no closing tag, no slash) for known void elements, so output is clean HTML5 whichever source form was used. Giving a void element children (possible when constructing `Html\Element` by hand) is an error rather than silent data loss.
* **Fragments** `<>...</>` produce an `Html\Fragment` with no wrapper element.
* **Whitespace** follows the JSX/Babel algorithm: split each text run into lines, trim leading whitespace on every line but the first and trailing whitespace on every line but the last, drop blank lines, and join the surviving lines with a single space. The net effect is that indentation/newlines between block elements vanish while a meaningful single space between inline content is preserved.
* **Character references**: markup text and literal attribute values decode HTML character references at compile time - the full WHATWG named set (semicolon-terminated forms only) plus numeric `&#8212;` / `&#x2014;`. The named list is *frozen by the HTML standard*, so it is baked into the scanner as a generated table. Decoding is lenient like HTML and JSX: a bare `&`, an unknown name, or an invalid numeric form stays literal - `<p>fish & chips</p>` needs no escaping. The decoded value is an ordinary string escaped at render time, so `&amp;` round-trips and `&lt;script&gt;` can never smuggle real markup structure. Text decodes *after* whitespace normalization; comment contents and interpolated PHP strings are never decoded, and `token_get_all()` still sees the raw source.
* **Literal braces**: `{` begins interpolation, so a literal `{` in text is written as in HTML or JSX - a character reference (`&#123;` / `&lbrace;`) or an interpolated string (`{'{'}`). A lone `}` is already literal. No additional escape syntax (eg. Blade-style `@{{`) is introduced. A block of literal JSON/JS inside `<script>` wants the `Html\raw()` JSON-island pattern from Escape-by-default instead.
* **Comments**: `<!-- ... -->` is emitted as a literal HTML comment; `{/* ... */}` renders   nothing (source-only).
* **Doctype**: `<!DOCTYPE html>` is allowed in *content position* and emitted verbatim (case-insensitive, no interpolation). It does **not** begin a markup expression - a markup expression is a single root node, so a full document wraps it in a fragment, exactly as JSX handles sibling roots: `<> <!DOCTYPE html> <html>...</html> </>`. Position is not validated (browsers ignore a misplaced doctype; it is the author's responsibility). Other `<!` declarations (e.g. CDATA sections, which terminate at `]]>` and may contain `>`) are a parse error with a targeted message.

### 7. Element model and DOM interoperability

Markup builds a **lightweight, immutable value-object tree** (`Html\Element`,`Html\Fragment`), *not* PHP 8.4's `Dom\*` tree. Reasons:

1. **Document ownership** - a `Dom\Element` must be created from and owned by a `Dom\Document`; markup literals are free-floating values, which fights that model.
2. **Cost** - DOM nodes are heavyweight (parent pointers, owner document, namespaces, live collections); templating wants cheap throwaway nodes per value.
3. **`Html\raw()`** requires opaque byte passthrough to avoid confusion to end users, which a DOM cannot represent without parsing and reserializing (potentially altering it).
4. **Component contract** - returning `Html\Htmlable` (one method) is far lighter than returning a document-owned `Dom\Node`.

Interop is provided both ways: `Html\Element::toDom(?Dom\Document = null): Dom\DocumentFragment` (when no document is given, a fresh empty `Dom\HTMLDocument` is created to own the returned fragment), and a `Dom\Node` is accepted as a child/interpolation (serialized to `Html\Htmlable`).

One documented `toDom()` limitation: the rendered HTML is parsed as a fragment in "in body" context, so context-sensitive elements at the *top level* of the converted value (`<td>`, `<tr>`, `<caption>`, ...) are subject to the HTML parser's error recovery - convert the enclosing `<table>` instead.

### 8. New symbols

The runtime lives in a new always-enabled bundled extension, `ext/html`. It cannot be disabled (like `ext/random`): the syntax lowers into these symbols at compile time, so a build without them would parse markup and then fail at runtime.

All under the `Html\` namespace:

* Interface `Html\Htmlable extends Stringable` - the renderable contract. The one method a class writes is `toHtml(): Htmlable`, which returns markup (ultimately an `Element`/`Fragment`/`Raw`). The inherited `__toString(): string` requirement is satisfied automatically: a userland class that does not declare (or inherit) a `__toString` of its own receives a default implementation at class-link time - it serializes what `toHtml()` produces - exactly the way every enum receives `cases()`. So `echo`/`(string)` work on any markup value or component, while `strict_types` components can `return <div>...</div>;` directly and callers can still reach the object tree via `toHtml()`. A declared `__toString` wins for string casts; markup rendering always goes through `toHtml()`.
* Classes `Html\Element`, `Html\Fragment` (and `Html\Raw`, the opaque passthrough that backs `raw()`/`escape()`). All are `final`, immutable value objects with `readonly` properties (`$tag`, `$attributes`, `$children` / `$html`), and each has a `toDom(?Dom\Document = null): Dom\DocumentFragment` method.
* Attribute `Html\Slot` (`#[Html\Slot(?string $name = null)]`).
* Functions `Html\raw(string $html): Html\Htmlable`, `Html\escape(string $text): Html\Htmlable`.
* Function `Html\render_component()` - the runtime target a component tag lowers into (name resolution candidates, props, slots). It is a public function so the dispatch rules are testable and so generated code is honest, but user code is expected to write tags, not call it.
* Function `Html\render_dynamic()` - the runtime target a dynamic tag `<$tag>` lowers into: classifies the value by the element-vs-component rule and either constructs an `Html\Element` or dispatches through the `render_component()` machinery. Public for the same reasons.
* Dispatch hooks, each with a matching `unregister_*` returning `bool`: `Html\register_component_factory()`, `Html\register_component_invoker()`, `Html\register_component_decorator()`. Each takes an optional `?string $component = null` scoping the hook to one component. Registering a non-callable throws a `TypeError` immediately (as `spl_autoload_register()` does).

### Error handling

Compile-time violations (mismatched closing tag, misplaced `<slot:>` block, a stray `<` in markup text) throw the standard **`CompileError`** / **`ParseError`**, catchable exactly like any other PHP syntax error. At runtime, unsupported attribute/child value types throw **`TypeError`**; invalid dynamically-supplied tag/attribute names throw **`ValueError`**; dispatch failures (unknown component, non-`Htmlable` return, misconfigured slots) throw **`Error`**.

## Examples

The constructs above compose with no special cases. The examples below show how they combine in the scenarios everyday template code actually consists of.

### A page is one expression

A markup expression spans as many lines as it needs. It is an ordinary expression, so the statement simply ends at the `;` - there is no heredoc-style terminator and no directive to close - and PHP control flow stays available inside `{}` throughout:

```php
function ProductPage(Product $product, User $viewer): Html\Htmlable
{
    return <Layout title={$product->name}>
        <slot:header>
            <h1>{$product->name}</h1>
            {$product->onSale ? <span class="badge">Sale</span> : null}
        </slot:header>

        <p class="description">{$product->description}</p>
        <ul class="features">
            {array_map(fn($feature) => <li>{$feature}</li>, $product->features)}
        </ul>

        {$viewer->isAdmin() ? <a href={"/admin/products/{$product->id}"}>Edit</a> : null}
    </Layout>;
}
```

### Typed props - enums and `match`

Attribute expressions are arbitrary PHP, and component props are real, typed parameters - so a prop can be an enum, and its value can be computed inline with `match`:

```php
enum Variant: string
{
    case Primary = 'btn-primary';
    case Secondary = 'btn-secondary';
}

function Button(Variant $variant, #[Html\Slot] Html\Htmlable $label): Html\Htmlable
{
    return <button class={(match ($theme) {
        'brand' => Variant::Primary,
        default => Variant::Secondary,
    })->value}>{$label}</button>;
}

echo <Button variant={$theme}>Click me</Button>;

// <button class="btn-secondary">Click me</button>
```

A prop mismatch - a misspelled attribute name, a string where the enum is expected - is an ordinary `TypeError`, and because the signature is real, static analysers can report it before the code ever runs.

### JSON output in markup

As the `{` character in markup triggers PHP syntax, it is important to note the character can still be output in various ways - it is the same workaround JSX requires:

```php
echo <script type="application/ld+json">{Html\raw(json_encode($json))}</script>;

echo <script type="application/ld+json">{Html\raw('
    {
        "@type": "WebPage",
        "name": "PHP",
        "url": "https://php.net",
        "@context": "http://schema.org"
    }
')}</script>;

echo <div>{'{'}example{'}'}</div>;
```

### Components are testable values

A component call is an ordinary call and its result an ordinary value object, so a component is unit-tested directly: call it, assert against the tree - no rendering, no string matching, no DOM parsing:

```php
public function testSaleBadgeShownWhenOnSale(): void
{
    $el = ProductBadge(product: $this->saleProduct());

    $this->assertInstanceOf(Html\Element::class, $el);
    $this->assertSame('span', $el->tag);
    $this->assertSame('badge sale', $el->attributes['class']);
}
```

Views decompose the way any other code does: a class of small component methods, each testable in isolation, composed by a top-level method returning the final page.

### Framework components - a Livewire-style clock

In frameworks whose components are PHP classes that render themselves - Livewire is the clearest example - the class and its markup are forced apart today: the component holds the state and behaviour, while the HTML lives in a separate Blade file the class points to by name.

With markup expressions the same component could collapse into one self-contained class - `render()` returns the view instead of naming a file that contains it:

```php
class Clock extends Component
{
    public string $timezone = 'UTC';

    public function render(): Html\Htmlable
    {
        return <div wire:poll.1s>
            <span class="time">{now($this->timezone)->format('H:i:s')}</span>
        </div>;
    }
}
```

## Parsing the `<` ambiguity

`<` is heavily overloaded in PHP (`<`, `<=`, `<=>`, `<<`, `<>`, `<<<`), so this was the main design risk. It is resolved by the same technique JavaScript uses to tell regex from division: the scanner tracks whether it is in **operand position** (a value is expected) or **operator position** (a value just ended). Markup begins only when `<` is in operand position *and* is immediately followed by `[A-Za-z>]`, by `\` then a letter (a fully-qualified component tag such as `<\App\Card/>`), by `$` then a variable name, or by `{` (the dynamic tags `<$tag>` / `<{expr}>`):

* In `$a < $b` (or the no-space `$a <$b`), `<` follows an operand → comparison, unchanged.
* In `$a < \Foo::BAR`, `<` is in operator position → comparison, unchanged.
* In `return <div>`, `<` is in operand position followed by a letter → markup.

Because `<` followed by a letter, `$`, `{`, or `>` in operand position is *always a syntax error* in PHP today, reclaiming it does not change the meaning of any valid program. Every operand-ending context (`static`, `exit`, bare `yield`, postfix `++`/`--`, `::class`, closing `)`/`]`/`}`, heredoc terminators, string closers) is explicitly kept as a comparison, which compare the received value today and still do.

This is **implemented and verified**: the full Zend and PHP language test suites pass with **zero regressions** (6000+ tests), and `token_get_all()` continues to tokenize ordinary code identically (the operand/operator tracking is one extra store per token, with no measurable overhead).

## Backward Incompatible Changes

1. **`<>` operator** - `<>` is the legacy alias of `!=`. It is **preserved in infix position** (`$a <> $b` is still not-equal). `<>` is only reinterpreted as a fragment opener in **operand position**, where it is currently a syntax error - so no valid program needs changes.
2. **`<` in operand position followed by a letter** (or by `\`+letter for a fully-qualified component tag, or by `$`+name / `{` for a dynamic tag) - currently a syntax error in such positions; reclaimed for markup, so no valid program needs changes.
3. **New `Html\` namespace** - introduces `Html\Htmlable`, `Html\Element`, `Html\Fragment`, `Html\Raw`, `Html\Slot`, `Html\raw()`, `Html\escape()`. Top-level `Html\` follows the established precedent for bundled extensions (`Random\`, `Dom\`, `Uri\`), and real-world top-level `Html\` packages are rare. No existing keywords are reserved and no new global reserved words are introduced (the syntax uses bare `<` and a `slot:` form valid only inside markup).

### Impact on tooling

The scanner emits new `T_MARKUP_*` tokens (exposed as constants and through `token_get_all()`/`PhpToken`); ordinary code continues to tokenize identically. Tools that parse PHP source (nikic/php-parser, PHPStan, Psalm, php-cs-fixer, IDEs) will need grammar updates to understand markup expressions, as with any new syntax.

A working syntax-highlighting/IDE extension for VS Code was built alongside the reference implementation to validate that the syntax is toolable.

Because markup lowers to ordinary AST at compile time, everything downstream of the parser is unaffected: opcache caches and restores markup-containing files normally (verified, including the file cache), and reflection, serialization, `var_export()`, `clone` and `json_encode()` on the value objects behave as for any other class.

A related question is what this means for **template engines** (Blade, Twig, Latte). This RFC does not replace them, and does not aim to: a full engine is much more than syntax plus escaping - layout inheritance, a designer-facing dialect that is deliberately *not* PHP, sandboxed rendering of untrusted templates, context-aware escaping, fragment caching, and years of ecosystem conventions. Nothing here removes any of that; markup is opt-in syntax, and code that never uses it is untouched.

What this RFC offers the engines is a first-class substrate to build on. Today each engine maintains its own parser, its own escaping, and its own component model, because the language provides none. With markup native, an engine can adopt the shared foundation where it fits: `Html\Htmlable` is a common currency (a Blade view and a markup expression can accept and return the same interface and compose within one page); the dispatch hooks exist precisely so a framework can route component tags through its existing container and resolution logic; and an engine's component layer could compile *to* markup expressions, inheriting escape-by-default and end-to-end static analysis for free. The likely long-term shape is convergence rather than replacement: engines keep their higher-level features and progressively delegate parsing, escaping, and the value model to the language - much as database layers did not disappear when PDO shipped, but re-based onto it.

## Future Scope

Some natural extensions are deliberately left out of this RFC to keep its scope tight and expandable in the future; none is required for the feature to be useful:

* **Context-aware escaping** (URL/JS/CSS contexts), as in Go's `html/template` or Latte's escaping, - so that e.g. a `javascript:` URL in `href` is caught. This RFC escapes HTML context only; the value-tree model is designed so this can be layered on later without a syntax change. Some examples include:
  * A JavaScript escaping context inside `<script>` tags, `onload=""`, `onclick=""`, etc. attributes
  * A CSS escaping context inside `<style>` tags or `style=""` attributes
* **Lazy children** via closures, to allow a component to conditionally skip or defer
  rendering its body (today children are eagerly evaluated, as with any function call).
* **XML support** more in line with XHP's original design.
* **Control structures** in the markup syntax, such as Vue's `v-if`.
* **Dedicated template files**, potentially reigniting the `.phpc` file debate.

## Reference implementation status

* **Zero regressions** across the `Zend/tests`, `tests/`, `ext/tokenizer` and `ext/dom` suites (6000+ tests); 30+ dedicated `ext/html` tests (including bare/qualified/fully-qualified resolution and the dispatch hooks).
* **opcache-safe** - markup compiles to ordinary opcode literals that persist and restore through the file cache (verified).
* **Safe by construction** - dynamically-supplied tag/attribute names are validated so a spread key or `new Element($x)` can never break out of the markup; child recursion is depth-bounded.
* **No measurable cost to non-markup code** - the syntax itself is zero-cost versus the equivalent hand-written `Html\Element` objects (it compiles to exactly those), and the scanner change is within measurement noise on ordinary PHP.

## References

* JSX specification - https://facebook.github.io/jsx/
* Laravel Blade, Twig, Latte - escape-by-default precedent in PHP templating.

### History and related work

* XHP - https://en.wikipedia.org/wiki/XHP ; Hack/HHVM docs: https://docs.hhvm.com/hack/XHP/introduction/
* Facebook's original PHP 5 extension (archived): https://github.com/facebookarchive/xhp-php5-extension
* `phplang/xhp` - XHP for PHP 7+ as a community extension: https://github.com/phplang/xhp
* Internals discussion endorsing the XHP approach over an auto-escape flag (2018):  https://externals.io/message/91797
* Adjacent string-interpolation RFCs: https://wiki.php.net/rfc/arbitrary_string_interpolation , https://wiki.php.net/rfc/arbitrary_expression_interpolation
* Userland JSX-for-PHP experiment - https://github.com/attitude/phpx
