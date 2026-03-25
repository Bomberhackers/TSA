#!/usr/bin/env python3

def apply(config, args):
    config['baseimg'] = 'baserom.z64'
    config['myimg'] = 'build/bm64tsa.z64'
    config['mapfile'] = 'build/bm64tsa.map'
    config['source_directories'] = ['.']
    config["objdump_flags"] = ["-Mreg-names=32"]
