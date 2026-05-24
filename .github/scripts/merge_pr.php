<?php

declare(strict_types=1);

class Context {
    public string $github_output;
    public string $pr_number;
    public string $pr_sha;
    public string $pr_ref;
    public string $pr_repo_url;
    public string $pr_title;
    public string $pr_description;
    public string $target_sha;
    public string $target_ref;
    /** @var list<string> */
    public array $release_branches;
    public string $pr_first_sha;
}

function get_context(): Context {
    $context = new Context;

    $env_mapping = [
        'PR_DESCRIPTION' => 'pr_description',
        'PR_NUMBER' => 'pr_number',
        'PR_REF' => 'pr_ref',
        'PR_REPO_URL' => 'pr_repo_url',
        'PR_SHA' => 'pr_sha',
        'PR_TITLE' => 'pr_title',
        'TARGET_REF' => 'target_ref',
        'TARGET_SHA' => 'target_sha',
    ];

    foreach ($env_mapping as $env_name => $prop_name) {
        $value = getenv($env_name);
        if ($value === false) {
            throw new InvalidArgumentException("Missing env var $env_name");
        }
        $context->{$prop_name} = $value;
    }

    $context->release_branches = find_release_branches($context->target_ref);
    $context->pr_first_sha = trim(run_command("git log --reverse --format=%H {$context->target_sha}..{$context->pr_sha} | head -n1")->stdout);

    return $context;
}

class ProcessResult {
    public int $status = 0;
    public string $stdout = '';
    public string $stderr = '';
}

/** @param string|list<string> $cmd */
function run_command(string|array $cmd, ?string $failure_message = 'Unexpected error.'): ProcessResult {
    if (is_array($cmd)) {
        $cmd = implode(' ', array_map('escapeshellarg', $cmd));
    }
    $pipes = null;
    $result = new ProcessResult();
    $descriptor_spec = [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']];
    fwrite(STDERR, "::group::{$cmd}\n");

    $process_handle = proc_open($cmd, $descriptor_spec, $pipes);
    if ($process_handle === false) {
        throw new RuntimeException("Failed to execute command `$cmd`");
    }

    $stdin = $pipes[0];
    $stdout = $pipes[1];
    $stderr = $pipes[2];

    fclose($stdin);

    stream_set_blocking($stdout, false);
    stream_set_blocking($stderr, false);

    $stdout_eof = false;
    $stderr_eof = false;

    do {
        $read = [$stdout, $stderr];
        $write = null;
        $except = null;

        stream_select($read, $write, $except, 1, 0);

        foreach ($read as $stream) {
            if (false !== $chunk = fgets($stream)) {
                if ($stream === $stdout) {
                    $result->stdout .= $chunk;
                    fwrite(STDOUT, $chunk);
                } elseif ($stream === $stderr) {
                    $result->stderr .= $chunk;
                    fwrite(STDERR, $chunk);
                }
            }
        }

        $stdout_eof = $stdout_eof || feof($stdout);
        $stderr_eof = $stderr_eof || feof($stderr);
    } while(!$stdout_eof || !$stderr_eof);

    fclose($stdout);
    fclose($stderr);

    $result->status = proc_close($process_handle);

    fwrite(STDERR, "::endgroup::\n");

    if ($result->status) {
        fwrite(STDERR, "Status code: {$result->status}\n");
        if ($failure_message) {
            throw new RuntimeException($failure_message);
        }
    }

    return $result;
}

/** @param list<string> $args */
function try_run(array $args): bool {
    $result = run_command($args, failure_message: null);
    return $result->status === 0;
}

/** @param list<string> $args */
function run(array $args, string $failure_message = 'Unexpected error.'): bool {
    $result = run_command($args, $failure_message);
    return $result->status === 0;
}

function origin_branch_exists(string $branch): bool {
    return try_run(['git', 'show-ref', '--verify', '--quiet', "refs/remotes/origin/$branch"]);
}

function find_next_release_branch(string $current): ?string {
    if ($current === 'master') {
        return null;
    }

    if (!preg_match('(^PHP-(?<major>\d+)\.(?<minor>\d+)$)', $current, $matches)) {
        throw new RuntimeException("Unsupported target branch $current.");
    }

    $major = $matches['major'];
    $minor = $matches['minor'];

    $next = "PHP-$major." . ($minor + 1);
    if (origin_branch_exists($next)) {
        return $next;
    }

    $next = 'PHP-' . ($major + 1) . '.0';
    if (origin_branch_exists($next)) {
        return $next;
    }

    return 'master';
}

/** @return list<string> */
function find_release_branches(string $target): array {
    $branches = [$target];
    while (null !== $next = find_next_release_branch(end($branches))) {
        $branches[] = $next;
    }
    return $branches;
}

function merge_pr_into_target(Context $context): string {
    $author = trim(run_command(['git', 'log', '-1', '--format=%an <%ae>', $context->pr_first_sha])->stdout);
    $message = "{$context->pr_title} (GH-{$context->pr_number})";

    run(['git', 'checkout', '-B', $context->target_ref, "refs/remotes/origin/{$context->target_ref}"]);
    run(['git', 'merge', '--squash', $context->pr_sha],
        failure_message: "Failed to squash PR into {$context->target_ref}.");
    run(['git', 'commit', "--author=$author", '-m', $message, '-m', wrap_commit_message($context->pr_description)]);
    $squashed_sha = trim((string) shell_exec('git rev-parse HEAD'));

    return $squashed_sha;
}

function merge_upwards(Context $context): void {
    $branches = $context->release_branches;
    for ($i = 1; $i < count($branches); $i++) {
        $prev = $branches[$i - 1];
        $current = $branches[$i];
        run(['git', 'checkout', '-B', $current, "refs/remotes/origin/$current"]);
        run(['git', 'merge', '--no-ff', '--no-edit', $prev],
            failure_message: "Failed to merge $prev into $current.");
    }
}

enum PushPrBranchResult {
    case Success;
    case Rejected;
    case RemoteRejected;
}

function push_pr_branch(Context $context, string $new_commit): PushPrBranchResult {
    $result = run_command(['git', 'push', "--force-with-lease={$context->pr_ref}:{$context->pr_sha}", $context->pr_repo_url, "$new_commit:refs/heads/{$context->pr_ref}"], failure_message: null);
    if ($result->status === 0) {
        return PushPrBranchResult::Success;
    } else if (preg_match('(\[rejected\])', $result->stderr)) {
        return PushPrBranchResult::Rejected;
    } else {
        return PushPrBranchResult::RemoteRejected;
    }
}

function push_release_branches(Context $context): bool {
    return try_run(['git', 'push', '--atomic', 'origin', ...$context->release_branches]);
}

function revert_pr_branch(Context $context, string $expected_commit): void {
    run_command(['git', 'push', "--force-with-lease={$context->pr_ref}:$expected_commit", $context->pr_repo_url, "{$context->pr_sha}:refs/heads/{$context->pr_ref}"],
        failure_message: 'Failed to push release branches. Reverting PR branch also failed.');
}

function wrap_commit_message(string $message, int $width = 80): string {
    $lines = explode("\n", $message);
    $result = [];
    $code_section = false;

    foreach ($lines as $line) {
        if (preg_match('(^\s*```)', $line)) {
            $code_section = !$code_section;
            $result[] = $line;
            continue;
        }

        if ($code_section) {
            $result[] = $line;
            continue;
        }

        if ($line === '' || preg_match('(^\s)', $line)) {
            $result[] = $line;
            continue;
        }

        $result[] = wordwrap($line, $width, "\n", false);
    }

    return implode("\n", $result);
}

function main(): int {
    $github_output = getenv('GITHUB_OUTPUT');
    if ($github_output === false) {
        throw new InvalidArgumentException('Missing env var GITHUB_OUTPUT');
    }

    try {
        $context = get_context();

        $squashed_sha = merge_pr_into_target($context);
        merge_upwards($context);
        $push_pr_branch_result = push_pr_branch($context, $squashed_sha);
        if ($push_pr_branch_result === PushPrBranchResult::Rejected) {
            throw new RuntimeException('PR branch diverged.');
        } else if ($push_pr_branch_result === PushPrBranchResult::RemoteRejected) {
            // Contributor likely unchecked the "Allow edits by maintainers"
            // checkbox. Resume and close PR manually.
            file_put_contents($github_output, "close_pr=1\n", FILE_APPEND);
        }
        if (!push_release_branches($context)) {
            revert_pr_branch($context, $squashed_sha);
            throw new RuntimeException('Failed to push release branches.');
        }
    } catch (Throwable $e) {
        file_put_contents($github_output, "fail_reason<<EOF\n{$e->getMessage()}\nEOF\n", FILE_APPEND);
        fwrite(STDERR, "::error::{$e->getMessage()}\n");
        return 1;
    }

    return 0;
}

exit(main());
