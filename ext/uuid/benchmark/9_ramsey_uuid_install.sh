#!/usr/bin/env bash
set -e

ramsey_version="4.9.2"
ramsey_dir="$PROJECT_ROOT/app/ramsey_uuid"

if [[ -d "$ramsey_dir" ]]; then
    echo "ramsey/uuid is already installed"
    exit
fi

mkdir -p "$ramsey_dir"

sudo docker run --rm \
    --volume $PROJECT_ROOT:/code \
    --user $(id -u):$(id -g) \
    setup bash -c "\
        set -e \
        [[ -n '$GITHUB_TOKEN' ]] && composer config --global github-oauth.github.com '$GITHUB_TOKEN'; \
        composer require ramsey/uuid:$ramsey_version --ignore-platform-reqs --no-interaction --working-dir=/code/app/ramsey_uuid && \
        composer config platform-check false --working-dir=/code/app/ramsey_uuid && \
        composer dump-autoload --classmap-authoritative --no-interaction --working-dir=/code/app/ramsey_uuid"
