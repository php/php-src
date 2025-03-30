docker build -t php-dev-setup .
docker run --volume .:/app php-dev-setup
