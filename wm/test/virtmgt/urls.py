from django.conf.urls import patterns, include, url
from django.conf.urls.defaults import * 
from django.conf import settings
from virtmgt.views import home, res, work, newvirt, newvirt_result, mgtresource, noderesource, resource, jsonmgt, jsontree, jsonnode, createall, json, restart
# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    (r'^home/$', home),
    (r'^res/$', res),
    (r'^work/$', work),
    (r'^newvirt/$', newvirt),
    (r'^newvirt_result/$', newvirt_result),
    #(r'^mgt/$', mgt),
    #(r'^mgt/title$', title),
    #(r'^mgt/tree$', tree),
    (r'^mgtresource/$', mgtresource),
    (r'^noderesource/$', noderesource),
    (r'^resource/$', resource),
    #(r'^mgt/app$', app),
    (r'^ajax/tree/$', jsontree),
    (r'^ajax/jsonmgt/$', jsonmgt),
    (r'^ajax/jsonnode/$', jsonnode),
    (r'^ajax/createall/$', createall),
    (r'^ajax/json/$', json),
    (r'^ajax/restart/$', restart),
    # Examples:
    # url(r'^$', 'virtmgt.views.home', name='home'),
    # url(r'^virtmgt/', include('virtmgt.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),
    (r'^media/(?P<path>.*)$','django.views.static.serve',{'document_root':settings.STATIC_PATH}),
    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
)
