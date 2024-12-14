@echo off
del users.db3
del ships.db3
del locations.db3

copy users.db3.orig users.db3
copy ships.db3.orig ships.db3
copy locations.db3.orig locations.db3