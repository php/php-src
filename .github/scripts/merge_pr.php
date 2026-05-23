<?php

function try_run(array $args): bool {
    $command = implode(' ', array_map('escapeshellarg', $args));
    echo "> $command\n";
    passthru($command, $status);
    return $status === 0;
}

function run(array $args, ?string $failure_message = null) {
    if (!try_run($args)) {
        throw new RuntimeException($failure_message ?? 'Command failed');
    }
}

function origin_branch_exists(string $branch): bool {
    return try_run(['git', 'show-ref', '--verify', '--quiet', "refs/remotes/origin/$branch"]);
}

function find_next_release_branch(string $current): ?string {
    if ($current === 'master') {
        return null;
    }

    if (!preg_match('(^PHP-(?<major>\d+)\.(?<minor>\d+)$)', $current, $matches)) {
        throw new RuntimeException("Unsupported target branch $current");
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
    $author = trim((string) shell_exec('git log -1 --format=' . escapeshellarg('%an <%ae>') . ' ' . escapeshellarg($pr_first_sha)));

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

function push_pr_branch(string $url, string $branch, string $squashed_sha, string $original_sha) {
    run(['git', 'push', "--force-with-lease=$branch:$original_sha", $url, "$squashed_sha:refs/heads/$branch"],
        failure_message: 'Failed to push rebased PR branch.');
}

function push_release_branches(array $branches) {
    run(['git', 'push', '--atomic', 'origin', ...$branches],
        failure_message: 'Failed to push release branches.');
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
    $target = getenv('TARGET_BRANCH');
    $pr_number = getenv('PR_NUMBER');
    $pr_first_sha = getenv('PR_FIRST_SHA');
    $pr_sha = getenv('PR_SHA');
    $pr_ref = getenv('PR_REF');
    $pr_repo_url = getenv('PR_REPO_URL');
    $pr_title = getenv('PR_TITLE');
    $pr_description = getenv('PR_DESCRIPTION');

    $release_branches = find_release_branches($target);

    try {
        $squashed_sha = merge_pr_into_target($pr_sha, $pr_first_sha, $target, "$pr_title (GH-$pr_number)", $pr_description);
        merge_upwards($release_branches);
        push_pr_branch($pr_repo_url, $pr_ref, $squashed_sha, $pr_sha);
        push_release_branches($release_branches);
    } catch (Throwable $e) {
        if (false !== ($github_output = getenv('GITHUB_OUTPUT'))) {
            file_put_contents($github_output, "fail_reason<<EOF\n{$e->getMessage()}\nEOF\n", FILE_APPEND);
        }
        fwrite(STDERR, "::error::{$e->getMessage()}\n");
        return 1;
    }

    return 0;
}

exit(main());
