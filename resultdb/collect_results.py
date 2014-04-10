#
# Read papertrail json log messages and reconstruct messages
# sent by opencl-testsuite. 
#
# Usage e.g. 
# curl -s -H "X-Papertrail-Token: <api-token>" "https://papertrailapp.com/api/v1/events/search.json?q=(program:opencl-testsuite)" | python collect_results.py
#

import fileinput
import json

result_db_fname = "result_db.json"
reject_db_fname = "rejected_messages_db.json"

rejected = {}
with open(reject_db_fname, "r") as f:
	rejected = json.loads(f.read())

results = {}
with open(result_db_fname, "r") as f:
	results = json.loads(f.read())

class LogReader(object):

	def __init__(self, accept_db, reject_db):
		self.accept_db = accept_db
		self.reject_db = reject_db
		self.suspicious_messages = []
		self.total_log_lines = 0
		self.filtered_events = 0
		self.total_messages = 0
		self.rejected_lines = 0
		self.new_lines = 0
		self.old_lines = 0
		self.suspicious_lines = 0

	def stat_str(self):
		return ("Total log lines      : %i\n" % self.total_log_lines + 
				"Valid log lines      : %i\n" % self.filtered_events + 
				"Parsed messages      : %i\n" % self.total_messages + 
				"New result entries   : %i\n" % self.new_lines + 
				"Rejected entries     : %i\n" % self.rejected_lines + 
				"Already processed    : %i\n" % self.old_lines)


	def process_logs(self, log_events):
		self.total_log_lines = len(log_events)
		filtered_logs = (l for l in log_events if l['program'] == 'opencl-testsuite')
		collected_messages = {}
		sender_info = {}

		# collect all package parts to table
		# { hash : {0 : message_part, 1: message_part} }
		for event in filtered_logs:
			self.filtered_events += 1
			message = event['message']
			packet_header,payload = message.split(":", 1)
			hash_code,packet_index = packet_header.split("-", 1)
			hash_parts = collected_messages.get(hash_code, {}) 
			hash_parts[int(packet_index)] = payload
			collected_messages[hash_code] = hash_parts
			del event['message']
			sender_info[hash_code] = event

		# combine packets in order to reconstruct message pieces and store to result_db
		# { hash : message }
		self.total_messages = len(collected_messages)
		for hash_code,parts in collected_messages.iteritems():
			full_msg = ""
			for i in range(len(parts)): 
				full_msg += parts.get(i, "FAIL")
				full_msg = full_msg[:-1] # drop extra space whic comes from logger service..

			try:
				original_message = json.loads(full_msg)
				# sanity check that we need to have devices key in result map
				if 'devices' in original_message:
					self.accept(hash_code, 
						{ "sender" : sender_info[hash_code], "message" : original_message })
				else:
					self.reject(hash_code, 
						{ "sender" : sender_info[hash_code], "message" : original_message })
			except:
				self.reject(hash_code, { "sender" : sender_info[hash_code], "message" : parts })


	def accept(self, hash_code, message):
		if hash_code in self.accept_db:
			self.old_lines += 1
			old_message = self.accept_db[hash_code]
			# TODO: check old message sender info if match with new
			#       if not match, print error about old and new
		else:
			self.new_lines += 1
			self.accept_db[hash_code] = message

	def reject(self, hash_code, message):
		if hash_code in self.reject_db:
			self.old_lines += 1
		else:
			self.rejected_lines += 1
			self.reject_db[hash_code] = message



# read stdin and parse as json
logs = json.loads("".join(fileinput.input()))
events = logs['events']
del logs['events']

reader = LogReader(results, rejected)
reader.process_logs(events)

# write to result db...
with open(result_db_fname, "w") as f:
	f.write(json.dumps(results, sort_keys=True, indent=2))

with open(reject_db_fname, "w") as f:
	f.write(json.dumps(rejected, sort_keys=True, indent=2))

print "########################### STATS ###################################"
print "Log info:",logs
print "Total log event count:", len(events)
print reader.stat_str()
print "########################### ----- ##################################"
