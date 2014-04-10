import urllib2
try:
	import json
except:
	import simplejson as json
import sys

import logging
import logging.handlers
import os


def filter_result(test_output_json_file):
	"""Removes output of passed tests before sending.
	"""
	file_content = open(test_output_json_file).read()
	parsed_content = json.loads(file_content)
	filtered_tests = [ 
		{ u'code' : t['code'], u'name' : t['name'], u'output' : t['output'] if t['code'] == u'FAIL' else u"" } 
		for t in parsed_content['tests'] ]
	parsed_content['tests'] = filtered_tests
	return parsed_content


def send_running_results_gist(filtered_results):
	"""Sends string as anonymous gist to github.

	@return Url where to find posted gist.
	"""
	payload = { 'description' : 'OpenCL testsuite results.',
				'public' : True,
				'files' : { 'results.json' : { 'content' : filtered_results }}}

	response = ""
	try:
		response = urllib2.urlopen('https://api.github.com/gists', json.dumps(payload, sort_keys=True, indent=2))
	except:
		print >>sys.stderr,"Error: Could not send report!"
		raise

	parsed_response = None
	if 200 <= response.getcode() < 300:
		response_str = response.read()
		try:
			parsed_response = json.loads(response_str)
		except:
			print >>sys.stderr,"Error: Could not parse response:",response_str
			raise

	return parsed_response['html_url']


def main(argv):

	if len(argv) != 2:
		print "Usage: ",argv[0],"<results_json>"
		return

	filtered_results = filter_result(argv[1])

	# add info about system with results
	with open("system_info.json", "r") as f:
		device_list = json.loads(f.read())
		filtered_results['devices'] = device_list

	print "---------- Test results were ------"
	print json.dumps(filtered_results, sort_keys=True, indent=2)
	print "----------------- end -------------"

	# Github does not support searching anonymous gists yet so this didn't work after all
	# gist_url = send_running_results_gist(json.dumps(filtered_results, sort_keys=True, indent=2))
	# print "Your report was sent to:", gist_url

	# Send results to syslog logging service... please don't spam there other type of data
	syslogHandler = logging.handlers.SysLogHandler(address=('logs.papertrailapp.com', 50438))
	hash_key = os.urandom(16).encode('hex')
	try:
		packet_length = 8192
		message = json.dumps(filtered_results, sort_keys=True)
		part_count = len(message)/packet_length + 1

		for i in range(part_count):
			start = i*packet_length
			end = start+packet_length
			syslogHandler.emit(logging.LogRecord("", logging.INFO, "", 0, "opencl-testsuite:" + hash_key + "-" + str(i) + ":" + message[start:end], {}, None))

	except:
		print >>sys.stderr,"Error: Could not send results to logging service."
		print >>sys.stderr,"Please send it as email to mikael.lepisto@vincit.fi"
		raise

	print "Submitting results was success. It was sent to logging service, where it will be stored few days. If you like to comment about entry with following code you can refer the result:",hash_key

if __name__ == "__main__":
	main(sys.argv)

