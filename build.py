#!/usr/bin/env python

import argparse
import sys
import os

from jinja2 import Environment, FileSystemLoader
import yaml


def calculate_metavars(params):
    params['extra_sources'] = ['hardware.c']
    params['extra_cdefs'] = []
    params['extra_ldflags'] = []

    if params.get('debug', False):
        if 'uart' in params and params['uart']['type'] == 'soft':
            params['extra_sources'].append('soft_uart.c')
        if 'uart' in params and params['uart']['type'] == 'hard':
            params['extra_sources'].append('hard_uart.c')

    if 'battery' in params:
        params['extra_sources'].append('battery.c')

    if 'temp' in params and params['temp']['type'] == 'DHT11':
        params['extra_sources'].append('dht11.c')

    if params.get('debug', False):
        params['extra_cdefs'].append('-DDEBUG')
        params['extra_cdefs'].append('-Wl,-u,vfprintf')
        params['extra_ldflags'].append('-lprintf_flt -lm')

    if 'switch' in params:
        params['extra_sources'].append('switch.c')

    return params


def template_file(source_file, dest_file, params):
    print 'Templating %s -> %s' % (source_file, dest_file)

    base_dir = os.path.dirname(source_file)
    template_file = os.path.basename(source_file)

    env = Environment(loader=FileSystemLoader(base_dir))
    template = env.get_template(template_file)

    with open(dest_file, 'w') as f:
        f.write(template.render(**params))


def main(rawargs):
    aparser = argparse.ArgumentParser(
        description='Build template build files')
    aparser.add_argument('profile',
                         help='profile to load')

    args = aparser.parse_args(rawargs)

    base_dir = os.path.dirname(__file__)
    profile_dir = os.path.join(base_dir, 'profiles')
    template_dir = os.path.join(profile_dir, 'templates')
    profile = os.path.join(profile_dir, args.profile)

    if not profile.endswith('.yaml'):
        profile += '.yaml'

    if not os.path.exists(profile):
        print 'Cannot find profile "%s"' % profile
        sys.exit(1)

    with open(profile, 'r') as f:
        params = yaml.load(f)

    params = calculate_metavars(params)
    params['source_file'] = os.path.basename(args.profile)

    for file in ['hardware.c.j2', 'hardware.h.j2', 'hardware.mk.j2']:
        template_file(os.path.join(template_dir, file),
                      os.path.join(base_dir, file.replace('.j2', '')),
                      params)


if __name__ == '__main__':
    main(sys.argv[1:])
