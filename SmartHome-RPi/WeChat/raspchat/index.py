#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import web
import time
import hashlib
from lxml import etree
import urllib,urllib2
import json

urls = (
'/weixin','WeixinInterface'
)
 

def _check_hash(data):
    #sha1加密算法
    signature=data.signature
    timestamp=data.timestamp
    nonce=data.nonce
    #自己的token
    token="wechatrpi" #这里改写你在微信公众平台里输入的token
    #字典序排序
    list=[token,timestamp,nonce]
    list.sort()
    sha1=hashlib.sha1()
    map(sha1.update,list)
    hashcode=sha1.hexdigest()
    #如果是来自微信的请求，则回复True
    if hashcode == signature:
        return True
    return False

    
class WeixinInterface:

    def _reply_text(self, toUser, fromUser, msg):
        return self.render.reply_text(toUser, fromUser, int(time.time()), msg)

    def _reply_image(self, toUser, fromUser, media_id):
        return self.render.reply_image(toUser, fromUser, int(time.time()), media_id)

    def __init__(self):
        self.app_root = os.path.dirname(__file__)
        self.templates_root = os.path.join(self.app_root, 'templates')
        self.render = web.template.render(self.templates_root)
 
    def GET(self):
        #获取输入参数
        data = web.input()
        if _check_hash(data):
            return data.echostr
    def get_media_ID(path,acces_token):
        """
        上传加过v的图片到微信服务器并返回对应id和url
        :param path 文件名称
        :arg acces_token token值
        """
        WeiXin_ImgUrl='https://api.weixin.qq.com/cgi-bin/media/upload' #临时素材URL
        WeiXin_Final_ImgUrl='https://api.weixin.qq.com/cgi-bin/material/add_material'#永久素材URL
        print('upload img ')
        payload_img={
            'access_token':acces_token,
            'type':'image',
            'media':open(path,'rb')
        }
        #data ={'media':open(path,'rb')}
        r=requests.post(url=WeiXin_ImgUrl,access_token=acces_token,type='image',media=open(path,'rb'))
        #r=requests.post(url=WeiXin_ImgUrl,params=payload_img,files=data)
        dict =r.json()
        print(dict)
        return dict#['media_id']
    
    def POST(self):
        '''
        str_xml = web.data()
        doc = etree.fromstring(str_xml)
        msgType = doc.find('MsgType').text
        fromUser = doc.find('FromUserName').text
        toUser = doc.find('ToUserName').text
        '''
        str_xml = web.data() #获得post来的数据
        xml = etree.fromstring(str_xml)#进行XML解析
        content=xml.find("Content").text#获得用户所输入的内容
        msgType=xml.find("MsgType").text
        fromUser=xml.find("FromUserName").text
        toUser=xml.find("ToUserName").text
        
        if content == "help":
            return self.render.reply_text(fromUser,toUser,int(time.time()),u"1.回复TAH获取当前温湿度"+'\n'
                                          +u"2.回复photo拍摄当前状况"+'\n'
                                          +u"3.回复lamp查看灯况"+'\n'
                                          +u"4.回复lampon开灯"+'\n'
                                          +u"5.回复lampoff关灯"+'\n'
                                          +u"6.其他回复尽情期待！"+ '\n')

        elif (content == "photo"):

            '''
            Imageurl = "https://api.weixin.qq.com/cgi-bin/media/upload?access_token=" + Access_Token + "&type=image"
            GetMediaID = os.system('curl -F media=@CurrentPhoto.jpg %s' %Imageurl)
            print(GetMediaID)
            a = GetMediaID.find("media_id")
            b = GetMediaID.find('","',a)
            MediaID = GetMediaID[a+11:b]
            print(MediaID)
            '''
            '''
            dict = self.get_media_ID("CurrentPhoto.jpg",Access_Token)
            xml_rep2 = "<xml>" \
                       "<ToUserName><![CDATA[%s]]></ToUserName>" \
                       "<FromUserName><![CDATA[%s]]></FromUserName>" \
                       "<CreateTime>%s</CreateTime>" \
                       "<MsgType><![CDATA[image]]></MsgType>" \
                       "<Image>" \
                       "<MediaId><![CDATA[%s]]></MediaId>"\
                       "</Image>"\
                        "</xml>"

            response = make_response(xml_rep2 % (fromu, tou, str(int(time.time())), dict['media_id']))
            response.content_type = 'application/xml'

            print response
            '''
            '''
            test_data = {'access_token':Access_Token,
                         'type':'image',
                         'media':open("CurrentPhoto.jpg","rb")}
            test_data_urlencode = urllib.urlencode(test_data)
            requrl = "https://api.weixin.qq.com/cgi-bin/media/upload"
            req = urllib2.Request(url = requrl,data =test_data_urlencode)
            res_data = urllib2.urlopen(req)
            res = res_data.read()
            print res
            '''

            os.system("sudo ./CameraPhoto.sh")
            time.sleep(1)
            req = urllib2.Request("https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=wx5451e98f3d7a3d12&secret=a8c0f91c7c89241e392afac35faa7ac3")
            res = urllib2.urlopen(req)
            res = res.read()
            print(res+'\n')
            a = res.find('":"')
            b = res.find('","')
            Access_Token = res[a+3:b]
            print(Access_Token+'\n')
            
            #Access_Token = "5_fEtrMFOQutjixCiDyaiWjj8LIrgCF7HN4iaPKpNqGB2bEBkNIg8Fc7iXAche_ELp8eNzVUC3nPYQF0wMgvnxbrMVAhxN77Br-LfydjrGhb48FubioTdr7huJeIOSNF9NHn1ikemkAAz6GiveYUNhAJARNG"
            url_media = "https://api.weixin.qq.com/cgi-bin/media/upload?access_token=" + Access_Token + "&type=image"
            req = "curl -o mediaid.txt -F media=@CurrentPhoto.jpg "+ '"' +url_media+ '"'
            #print(req)
            os.system(req)
            with open ("mediaid.txt","r") as f:
                res = f.read()
                print('txt='+res+'\n')
                a = res.find("media_id")
                b = res.find('","',a)
                MediaID = res[a+11:b]
                print('id='+MediaID+'\n')
                return self.render.reply_image(fromUser,toUser,int(time.time()), MediaID)

        elif (content == "TAH"):
            with open ("TAH.txt","r") as f:
                data = f.read()
                a = data.find("Wendu")
                b = data.find("Shidu")
                data_t = data[a+7:a+10]
                data_h = data[b+7:b+10]
                return self.render.reply_text(fromUser,toUser,int(time.time()),u"温度："+data_t + 'C\n'+u"湿度："+data_h+'%')
        elif (content == "lamp"):
            os.system("sudo ./LampRead")
            with open ("Lamp.txt","r") as f:
                data = f.read()
                if data=='1':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"当前状态：On")
                elif data=='0':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"当前状态：Off")
            os.remove("Lamp.txt")
        elif (content == "lampon"):
            os.system("sudo ./LampOn")
            with open ("Lamp.txt","r") as f:
                data = f.read()
                if data=='1':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"灯已开启！")
                elif data=='0':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"未知错误，请重试！")
            os.remove("Lamp.txt")
        elif (content == "lampoff"):
            os.system("sudo ./LampOff")
            with open ("Lamp.txt","r") as f:
                data = f.read()
                if data=='1':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"未知错误，请重试！")
                elif data=='0':
                    return self.render.reply_text(fromUser,toUser,int(time.time()),u"灯已关闭！")
            os.remove("Lamp.txt")
        else:    
            return self.render.reply_text(fromUser,toUser,int(time.time()),u"接收到的文字："+content+'\n'+u"回复help查看帮助"+ '\n') 
        

application = web.application(urls, globals())
if __name__ == "__main__":
    application.run()
   # application = web.application(urls, globals()).wsgifunc()
