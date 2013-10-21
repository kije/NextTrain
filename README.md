NextTrain – Server
==================

This is the Server side part of NextTrain.
Due to the limited **JSON support** of the Pebble SDK and httpebble, and to reduce the number of requests made from Pebble, the requests to the SBB and Google Geocode API are made this server script.

## Requirement
This is the Redis-driven version of NextTrainServer. You will need [Predis][1]. If you can't use Redis, check out the [MySQL version][2].

[1]: https://github.com/nrk/predis
[2]: https://github.com/kije/NextTrain/tree/server_mysql