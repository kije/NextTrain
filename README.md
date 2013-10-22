NextTrain – Server
==================

This is the Server side part of NextTrain.

Due to the limited **JSON support** of the Pebble SDK and httpebble, and to reduce the number of requests made from Pebble, the requests to the SBB and Google Geocode API are made this server script.

## Requirement
You will need [Predis][1] in order to get this working. There will be a MySQL version in the future.


## Installation
First, install [Predis][1]. Then, clone this repository and run:

    git checkout server

Now, customize the paths in <code>NextTrainServer/inc/db.php</code> and <code>NextTrainServer/inc/db_connect.php</code>.



[1]: https://github.com/nrk/predis