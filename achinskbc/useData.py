import webapp2
from google.appengine.ext import ndb

from datetime import datetime, timedelta
import json
#import cgi
#import urllib

class temperatureData(ndb.Model):
    """Models an individual Guestbook entry with author, content, and date."""
    time = ndb.DateTimeProperty(auto_now_add=True)
    t1 = ndb.FloatProperty(indexed=False)
    t2 = ndb.FloatProperty(indexed=False)
    t3 = ndb.FloatProperty(indexed=False)
    t4 = ndb.FloatProperty(indexed=False)

class putData(webapp2.RequestHandler):

    def get(self):
      if self.request.get('key') == "PASSWORDHERE":
        v1 = self.request.get('t1')
        v2 = self.request.get('t2')
        v3 = self.request.get('t3')
        v4 = self.request.get('t4')

        if v1 == "-127.0" or not v1:
          v1 = None
        else:
          v1 = float( v1 )

        if v2 == "-127.0" or not v2:
          v2 = None
        else:
          v2 = float( v2 )

        if v3 == "-127.0" or not v3:
          v3 = None
        else:
          v3 = float( v3 )

        if v4 == "-127.0" or not v4:
          v4 = None
        else:
          v4 = float( v4 )

        thisTemp = temperatureData( t1 = v1,
                                    t2 = v2,
                                    t3 = v3,
                                    t4 = v4 )
        thisTemp.put()
        self.response.out.write("OK")
      else:
        self.response.set_status(401)

class getData(webapp2.RequestHandler):

    def get(self):
        week = temperatureData.query(temperatureData.time >= ( datetime.now()-timedelta(days=7) ) ).order(temperatureData.time)
        self.response.headers['Content-Type'] = 'application/json'
        weekToJson = []
        for thisVal in week:
            weekToJson.append({'time': thisVal.time.isoformat(), 't1': thisVal.t1, 't2': thisVal.t2, 't3': thisVal.t3})
        self.response.out.write(json.dumps(weekToJson))



application = webapp2.WSGIApplication([
    ('/putData', putData),
    ('/getData', getData),
], debug=True)