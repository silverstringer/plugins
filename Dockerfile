# Сборка ---------------------------------------

# В качестве базового образа для сборки используем gcc:latest
FROM gcc:latest as build

# Установим рабочую директорию для сборки проекта
WORKDIR /app
# Скопируем директорию /src в контейнер
ADD ./src /app/src
ADD ./library /app/library
ADD CMakeLists.txt /app
ADD .gitmodules /app
ADD ./build.sh /app



# Запуск ---------------------------------------

# В качестве базового образа используем ubuntu:latest
FROM ubuntu:latest

RUN apt-get update && \
	apt-get install -y build-essential git cmake autoconf libtool pkg-config

# Добавим пользователя, потому как в Docker по умолчанию используется root
# Запускать незнакомое приложение под root'ом неприлично :)
RUN groupadd -r sample && useradd -r -g sample sample
USER sample

WORKDIR /app

# Скопируем приложение со сборочного контейнера в рабочую директорию
COPY --from=build /app .
USER root 
RUN /bin/bash -c 'chmod +x ./build.sh'                                                
#CMD [ "/bin/sh", "/app/build.sh" ]   
ENTRYPOINT ["/bin/bash","./build.sh","-start"]

