# Copyright (c) 2019 Cable Television Laboratories, Inc.
# Licensed under the BSD+Patent (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#          https://opensource.org/licenses/BSDplusPatent
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from bottle import abort,run,install,request,route,response
from datetime import datetime
from functools import wraps
import json
import logging
from logging.handlers import SysLogHandler
from optparse import OptionParser
import os

#===================
# Settings
#
appName = 'wccf_rx_rest'
logFacility = logging.handlers.SysLogHandler.LOG_LOCAL5
logLevel = logging.DEBUG
#
#===================

# Global used to pass CLI switches to functions
cliParams = {}

# set up the logger
logger = logging.getLogger(appName)
logger.setLevel(logLevel)

syslog_handler = SysLogHandler(address='/dev/log', facility=logFacility)
formatter = logging.Formatter('%(levelname)s: %(msg)s')
syslog_handler.setLevel(logLevel)
syslog_handler.setFormatter(formatter)
logger.addHandler(syslog_handler)

def log_to_logger(fn):
    '''
    Wrap a Bottle request so that a log line is emitted after it's handled.
    '''
    logger = logging.getLogger('wccf_rx_rest')
    @wraps(fn)
    def _log_to_logger(*args, **kwargs):
        request_time = datetime.now()
        actual_response = fn(*args, **kwargs)
        logger.info('%s %s %s %s %s' % (request.remote_addr,
                                        request_time,
                                        request.method,
                                        request.url,
                                        response.status))
        return actual_response
    return _log_to_logger

install(log_to_logger)

# TODO: Due to a problem with curllib, this was changed to support
# both a put and a post.  Need to change to just POST 
@route('/wccf/rec/<APMAC>',method='POST')
@route('/wccf/rec/<APMAC>',method='PUT')
def receiveFile(APMAC):
    logger.debug('BEGIN receiving file...'+APMAC)
    logger.debug('request content-type: ' + request.content_type)
    fn=request.query.fn
    logger.debug('fn='+fn)
    outputPath=cliParams['outputPath']
    oPath = os.path.join(outputPath,APMAC)
    if not os.path.exists(oPath):
        os.makedirs(oPath)
    for k in request.headers:
        logger.debug(k + ' - ' + request.headers[k])
    try:
        jout = json.dumps(request.json)
    except:
        logger.error('Error processing attachement for fn ' + fn + ', check f rmat of incoming json or headers') 
        abort(500,'Error processing attachement for fn '+ fn + ', check format of incoming json or headers')
    logger.debug('length of json: ' + str(len(jout)))
    if jout == 'null':
        logger.error('Zero length file or no json content received for %s.  No file written' %(fn) )
        abort(500,'Zero length file or no json content received for %s.  No file written' %(fn) )
    f=open(os.path.join(oPath,fn),'w')
    f.write(jout)
    f.close()

# Main
# Handle the CLI parameters
parser = OptionParser()
parser.add_option('-o', '--outputpath',
                  action='store', 
                  dest='outputPath', 
                  default='/tmp',
                  help='output path for file storage [default: %default]')
parser.add_option('-p', '--port',
                  action='store',
                  dest='httpPort',
                  default=8080,
                  help='Port for the local HTTP server [default: %default]')
parser.add_option('-H', '--hostname',
                  action='store',
                  dest='httpHost',
                  default='localhost',
                  help='Hostname used for local http server (IP or hostname) [default: %default]')

(options, args) = parser.parse_args()

outputPath = options.outputPath
httpPort = options.httpPort
httpHost = options.httpHost

# Set the global parameters needed by route functions
#
cliParams['outputPath']=outputPath

run(server='paste', host=httpHost, port=httpPort, debug=(logLevel==logging.DEBUG))
