TEMPLATE = subdirs

 SUBDIRS = \
    Examples \
    libQt6FFmpeg

 # where to find the sub projects - give the folders
 libQt6FFmpeg.subdir = libQt6FFmpeg
 Examples.subdir  = examples/test

 # what subproject depends on others
 Examples.depends = libQt6FFmpeg
