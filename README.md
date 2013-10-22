NextTrain – Server
==================

This is the Server side part of NextTrain.

Due to the limited **JSON support** of the Pebble SDK and httpebble, and to reduce the number of requests made from Pebble, the requests to the SBB and Google Geocode API are made this server script.

## Requirement
You will need [Predis][1] in order to get this working. There will be a MySQL version in the future.

[1]: https://github.com/nrk/predis
