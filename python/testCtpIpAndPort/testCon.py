#-*- coding: UTF-8 -*-   
# ���ļ��ж�ȡ����
# http://blog.csdn.net/yiluochenwu/article/details/23515923
# [python]ʹ��xml.etree.ElementTree����xml���нڵ�
#
import xml.etree.ElementTree as ET  
  
#ȫ��Ψһ��ʶ  
unique_id = 1  
  
#�������еĽڵ�  
def walkData(root_node, level, result_list):  
    global unique_id  
    temp_list =[unique_id, level, root_node.tag, root_node.attrib]  
    #temp_list =[root_node.attrib]  
    result_list.append(temp_list)  
    unique_id += 1  
      
    #����ÿ���ӽڵ�  
    children_node = root_node.getchildren()  
    if len(children_node) == 0:  
        return  
    for child in children_node:  
        walkData(child, level + 1, result_list)  
    return  
  
#���ԭʼ����  
#out:  
#[  
#    #ID, Level, Attr Map  
#    [1, 1, {'ID':1, 'Name':'test1'}],  
#    [2, 1, {'ID':1, 'Name':'test2'}],  
#]  
def getXmlData(file_name):  
    level = 1 #�ڵ����ȴ�1��ʼ  
    result_list = []  
    root = ET.parse(file_name).getroot()  
    walkData(root, level, result_list)  
  
    return result_list  
  
if __name__ == '__main__':  
    file_name = 'E:\\SmartGit\\TradeBlazerWork\\python\\testCtpIpAndPort\\brokers.xml'  
    R = getXmlData(file_name)  
    for x in R:  
        print (x)  
    pass  
    
#mydata = getXmlData('E:\\SmartGit\\TradeBlazerWork\\python\\testCtpIpAndPort\\brokers.xml')
#print(mydata)