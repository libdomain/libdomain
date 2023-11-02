# Running test suite locally
To run tests locally you first have to create data-volume, you need to run following command from libdomain's repository top folder:
```
docker volume create --driver local --opt type=none --opt device=`pwd` --opt o=bind data-volume
```
Next you need to build docker images, this can be done using following command:
```
docker compose -f docker-compose.openldap.yml -f docker-compose.yml build --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) --build-arg DISTR=alt:p10
```
After that you can run test by executing:
```
docker compose -f docker-compose.openldap.yml -f docker-compose.yml up --abort-on-container-exit --attach-dependencies
```

