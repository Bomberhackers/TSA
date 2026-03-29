#!/usr/bin/env python3

def apply(config, args):
    config['baseimg'] = 'baserom.decomp.us.z64'
    config['myimg'] = 'build/us/bm64tsa.decomp.us.z64'
    config['mapfile'] = 'build/us/bm64tsa.decomp.us.map'
    config['source_directories'] = ['.']
    config["objdump_flags"] = ["-Mreg-names=32"]
