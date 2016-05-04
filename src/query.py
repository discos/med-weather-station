#!/usr/bin/python

import argparse



def main():

    #Adding command line options
    parser=argparse.ArgumentParser(description="Allows to query the weather station archive")
    parser.add_argument('-d',action='store_true',help="enable debug messages",dest='debug')

    parser.add_argument('-t', '--get_templates', action='store_true',
                        dest="get_templates",
                        help="copy user templates into supplied directory, does not generate any schedule. Creates directory if it does not exist.")
    parser.add_argument('-c', '--configuration_file', dest='configuration_file',
                        default='configuration.txt',
                        help='user schedule configuration input file.')
    parser.add_argument('-f', action='store_true', dest='force',
                        help='force override of existing files')
    parser.add_argument('--version', action='store_true', dest='show_version',
                        help='print version information and exit')
    parser.add_argument('directory', default=".", nargs='?',
                        help="directory for schedule files or user templates")


    #parsing command line arguments
    ns = parser.parse_args()
    if ns.show_version:
        print "basie version: %s" % (VERSION,)
        sys.exit()




if __name__ == '__main__':
    main()
