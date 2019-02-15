# Shared Memory Operations Extension to PHP

While developing a search deamon we needed a php based front end to communicate
the deamon via SHM. PHP already had a shared memory extension (sysvshm) written
by Christian Cartus <cartus@atrior.de>, unfortunately this extension was designed
with PHP only in mind and offers high level features which are extremely
bothersome for basic SHM we had in mind.  After spending a day trying to reverse
engineer and figure out the format of sysvshm we decided that it would be much
easier to add our own extension to php for simple SHM operations, we were right :)).
