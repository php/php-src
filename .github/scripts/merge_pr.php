<?php

class ProcessResult {
    public $status;
    public $stdout;
    public $stderr;
}

function run_command(string|array $cmd, ?string $failure_message = 'Unexpected error.'): ProcessResult {
    if (is_array($cmd)) {
        $cmd = implode(' ', array_map('escapeshellarg', $cmd));
    }
    $pipes = null;
    $result = new ProcessResult();
    $descriptor_spec = [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']];
    fwrite(STDERR, "> $cmd\n");
    $process_handle = proc_open($cmd, $descriptor_spec, $pipes);

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
            $chunk = fgets($stream);
            if ($stream === $stdout) {
                $result->stdout .= $chunk;
                fwrite(STDOUT, $chunk);
            } elseif ($stream === $stderr) {
                $result->stderr .= $chunk;
                fwrite(STDERR, $chunk);
            }
        }

        $stdout_eof = $stdout_eof || feof($stdout);
        $stderr_eof = $stderr_eof || feof($stderr);
    } while(!$stdout_eof || !$stderr_eof);

    fclose($stdout);
    fclose($stderr);

    $result->status = proc_close($process_handle);

    if ($result->status) {
        fwrite(STDERR, "Status code: {$result->status}\n");
        if ($failure_message) {
            throw new RuntimeException($failure_message);
        }
    }

    return $result;
}

function try_run(array $args): bool {
    $result = run_command($args, failure_message: null);
    return $result->status === 0;
}

function run(array $args, ?string $failure_message = null): bool {
    $result = run_command($args, $failure_message ?? 'Unexpected error.');
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

function find_release_branches(string $target): array {
    $branches = [$target];
    while (null !== $next = find_next_release_branch(end($branches))) {
        $branches[] = $next;
    }
    return $branches;
}

function merge_pr_into_target(string $pr_sha, string $pr_first_sha, string $target, string $message, string $description): string {
    $author = trim(run_command(['git', 'log', '-1', '--format=%an <%ae>', $pr_first_sha])->stdout);

    run(['git', 'checkout', '-B', $target, "refs/remotes/origin/$target"]);
    run(['git', 'merge', '--squash', $pr_sha],
        failure_message: "Failed to squash PR into $target.");
    run(['git', 'commit', "--author=$author", '-m', $message, '-m', wrap_commit_message($description)]);
    $squashed_sha = trim((string) shell_exec('git rev-parse HEAD'));

    return $squashed_sha;
}

function merge_upwards(array $branches) {
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

function push_pr_branch(string $url, string $branch, string $new_commit, string $expected_commit) {
    $result = run_command(['git', 'push', "--force-with-lease=$branch:$expected_commit", $url, "$new_commit:refs/heads/$branch"], failure_message: null);
    if ($result->status === 0) {
        return PushPrBranchResult::Success;
    } else if (preg_match('(\[rejected\])', $result->stderr)) {
        return PushPrBranchResult::Rejected;
    } else {
        return PushPrBranchResult::RemoteRejected;
    }
}

function push_release_branches(array $branches): bool {
    return try_run(['git', 'push', '--atomic', 'origin', ...$branches]);
}

function revert_pr_branch(string $url, string $branch, string $new_commit, string $expected_commit) {
    run_command(['git', 'push', "--force-with-lease=$branch:$expected_commit", $url, "$new_commit:refs/heads/$branch"],
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
    $target_sha = getenv('TARGET_SHA');
    $target_ref = getenv('TARGET_REF');
    $pr_number = getenv('PR_NUMBER');
    $pr_sha = getenv('PR_SHA');
    $pr_ref = getenv('PR_REF');
    $pr_repo_url = getenv('PR_REPO_URL');
    $pr_title = getenv('PR_TITLE');
    $pr_description = getenv('PR_DESCRIPTION');
    $github_output = getenv('GITHUB_OUTPUT');

    try {
        $release_branches = find_release_branches($target_ref);
        $pr_first_sha = trim(run_command("git log --reverse --format=%H $target_sha..$pr_sha | head -n1")->stdout);

        $squashed_sha = merge_pr_into_target($pr_sha, $pr_first_sha, $target_ref, "$pr_title (GH-$pr_number)", $pr_description);
        merge_upwards($release_branches);
        $push_pr_branch_result = push_pr_branch($pr_repo_url, $pr_ref, $squashed_sha, $pr_sha);
        if ($push_pr_branch_result === PushPrBranchResult::Rejected) {
            throw new RuntimeException('PR branch diverged.');
        } else if ($push_pr_branch_result === PushPrBranchResult::RemoteRejected) {
            // Contributor likely unchecked the "Allow edits by maintainers"
            // checkbox. Resume and close PR manually.
            file_put_contents($github_output, "close_pr=1\n", FILE_APPEND);
        }
        if (!push_release_branches($release_branches)) {
            revert_pr_branch($pr_repo_url, $pr_ref, $pr_sha, $squashed_sha);
            throw new RuntimeException('Failed to push release branches.');
        }
    } catch (Throwable $e) {
        if ($github_output !== false) {
            file_put_contents($github_output, "fail_reason<<EOF\n{$e->getMessage()}\nEOF\n", FILE_APPEND);
        }
        fwrite(STDERR, "::error::{$e->getMessage()}\n");
        return 1;
    }

    return 0;
}

exit(main());
