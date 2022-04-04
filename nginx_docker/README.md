docker build -t webserver .
docker run -it --rm -d -p 8080:80 --name web webserver
docker exec -it `id_docker_ps` bash
