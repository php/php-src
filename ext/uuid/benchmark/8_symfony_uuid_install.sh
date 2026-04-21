#!/usr/bin/env bash
set -e

symfony_version="8.0.8"
symfony_dir="$PROJECT_ROOT/app/symfony_uuid"

if [[ -d "$symfony_dir" ]]; then
    echo "symfony/uid is already installed"
    exit
fi

mkdir -p "$symfony_dir"

sudo docker run --rm \
    --volume $PROJECT_ROOT:/code \
    --user $(id -u):$(id -g) \
    setup bash -c "\
        set -e \
        [[ -n '$GITHUB_TOKEN' ]] && composer config --global github-oauth.github.com '$GITHUB_TOKEN'; \
        composer require symfony/uid:$symfony_version --ignore-platform-reqs --no-interaction --working-dir=/code/app/symfony_uuid && \
        composer config platform-check false --working-dir=/code/app/symfony_uuid && \
        composer dump-autoload --classmap-authoritative --no-interaction --working-dir=/code/app/symfony_uuid"
