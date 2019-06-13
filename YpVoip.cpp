// CVoipDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "YpVoip.h"
#include "afxdialogex.h"
#include "CropType.h"
enum VOIP_LIST_REPORT_NAME
{
	VOIP_VIDEO_NAME = 0,
	VOIP_VIDEO_STATUS,
	VOIP_VIDEO_ID,
	VOIP_VIDEO_CREATER
};


YpVoip::YpVoip(CUserManager* pUserManager)
{
	m_pUserManager = pUserManager;
	m_pVoipManager = new XHVoipManager(this);
	if (m_ShowLiveDlg == NULL) {
		m_ShowLiveDlg = new CShowLiveDlg();
		m_ShowLiveDlg->addPictureControlListener(this);
		m_ShowLiveDlg->addShowLiveListener(this);
		m_ShowLiveDlg->Create(IDD_DIALOG_SHOW_LIVE, AfxGetMainWnd());
	}
	m_strTargetId = "";
	m_pSoundManager = new CSoundManager(this);
}

YpVoip::~YpVoip()
{
	if (m_pVoipManager != NULL)
	{
		delete m_pVoipManager;
		m_pVoipManager = NULL;
	}
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		delete m_pSoundManager;
		m_pSoundManager = NULL;
	}
	if (m_ShowLiveDlg != NULL)
	{
		delete m_ShowLiveDlg;
		m_ShowLiveDlg = NULL;
	}

}


// voip callback
/**
 * 被叫方响应
 * 被叫方收到主叫方的呼叫
 * @param fromID
 */
void YpVoip::onCalling(string fromID)
{
	CString str;
	str.Format("是否同意用户:%s请求视频通话", fromID.c_str());
	if (IDYES == AfxMessageBox(str, MB_YESNO))
	{
		m_bAudio = false;
		m_strTargetId = fromID;
		m_ShowLiveDlg->ShowWindow(SW_SHOW);
		m_pVoipManager->accept(fromID);
		m_bConnect = true;

		if (m_ShowLiveDlg->m_pDataShowView != NULL)
		{
			m_ShowLiveDlg->m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, false);
			m_ShowLiveDlg->m_pDataShowView->addUser(fromID, true);
			m_ShowLiveDlg->m_pDataShowView->setShowPictures();
		}
		startGetData((CROP_TYPE)m_pUserManager->m_ServiceParam.m_CropType, true);
		if (m_pSoundManager != NULL)
		{
			m_pSoundManager->startGetSoundData(true);
		}
	}
	else
	{
		m_pVoipManager->refuse();
	}
}

/**
* 被叫方响应语音呼叫
* 被叫方收到主叫方的呼叫
* @param fromID
*/
void YpVoip::onAudioCalling(string fromID)
{
	CString str;
	str.Format("是否同意用户:%s请求语音通话", fromID.c_str());
	if (IDYES == AfxMessageBox(str, MB_YESNO))
	{
		m_bAudio = true;
		m_strTargetId = fromID;
		CRect rect;
		m_ShowLiveDlg->m_bAudio = true;
		m_ShowLiveDlg->m_strAimID = fromID;
		m_ShowLiveDlg->ShowWindow(SW_SHOW);
		m_pVoipManager->accept(fromID);
		m_bConnect = true;

		if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
		{
			m_ShowLiveDlg->m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, false);
			m_ShowLiveDlg->m_pDataShowView->addUser(fromID, true);
			m_ShowLiveDlg->m_pDataShowView->setShowPictures();
		}
		//startGetData((CROP_TYPE)m_pUserManager->m_ServiceParam.m_CropType, true);
		if (m_pSoundManager != NULL)
		{
			m_pSoundManager->startGetSoundData(true);
		}
	}
	else
	{
		m_pVoipManager->refuse();
	}
}

/**
 * 被叫方响应
 * 主叫方在被叫方接听之前挂断（通话被取消）
 * @param fromID
 */
void YpVoip::onCancled(string fromID)
{
	stopGetData();
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	m_strTargetId = "";
}

/**
 * 主叫方响应
 * 被叫方拒绝接通（通话被拒绝）
 * @param fromID
 */
void YpVoip::onRefused(string fromID)
{
	stopGetData();
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	m_strTargetId = "";
	AfxMessageBox("对方拒绝接通");
}

/**
 * 主叫方响应
 * 被叫方线路忙（对方通话中）
 * @param fromID
 */
void YpVoip::onBusy(string fromID)
{
	//关闭
	stopGetData();
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	m_strTargetId = "";
	AfxMessageBox("对方线路忙");
}

/**
 * 通话中错过的来电
 */
void YpVoip::onMiss(CIMMessage* pMsg)
{
}

/**
 * 主叫方响应
 * 被叫方接通（通话开始）
 * @param fromID
 */
void YpVoip::onConnected(string fromID)
{
	//添加显示对方界面
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_strTargetId = fromID;
		m_ShowLiveDlg->m_pDataShowView->addUser(fromID, true);
		m_ShowLiveDlg->m_pDataShowView->changeShowStyle(m_pUserManager->m_ServiceParam.m_strUserId, false);
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	//设置插入数据
	m_bInsertData = true;
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->startGetSoundData(true);
	}
	m_bConnect = true;
}

/**
 * 对方已经挂断
 * @param fromID
 */
void YpVoip::onHangup(string fromID)
{
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopGetSoundData();
	}

	if (m_pVoipManager != NULL)
	{
		if (m_bConnect)
		{
			m_pVoipManager->hangup(0);
		}
		else
		{
			m_pVoipManager->cancel();
		}
		m_bConnect = false;
	}
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	m_strTargetId = "";
}

/**
 * voip报错
 * @param errorCode
 */
void YpVoip::onError(string errorCode)
{
	//开启自己
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopGetSoundData();
	}
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->hangup(0);
	}
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	//断开连接
	CString strErr;
	strErr.Format("err:%s", errorCode.c_str());
	AfxMessageBox(strErr);
	m_strTargetId = "";
}

/**
 * 收到实时数据
 * @param data
 */
void YpVoip::onReceiveRealtimeData(uint8_t* data, int length)
{
}

int YpVoip::getVideoRaw(string strUserId, int w, int h, uint8_t* videoData, int videoDataLen)
{
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->drawPic(FMT_YUV420P, m_strTargetId, w, h, videoData, videoDataLen);
	}
	return 0;
}

/**
 * 数据传输模式切换
 * @param state
 */
void YpVoip::onTransStateChanged(int state)
{
}

void YpVoip::stopLive()
{
	stopGetData();
	if (m_pSoundManager != NULL)
	{
		m_pSoundManager->stopGetSoundData();
	}

	if (m_pVoipManager != NULL)
	{
		if (m_bConnect)
		{
			m_pVoipManager->hangup(1);
		}
		else
		{
			m_pVoipManager->cancel();
		}
		m_bConnect = false;
	}
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->removeAllUser();
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
	m_ShowLiveDlg->ShowWindow(SW_HIDE);
	m_strTargetId = "";
}

void YpVoip::addUpId()
{
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, false);
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
}

void YpVoip::insertVideoRaw(uint8_t* videoData, int dataLen, int isBig)
{
	if (m_pVoipManager != NULL && m_bAudio == false)
	{
		m_pVoipManager->insertVideoRaw(videoData, dataLen, isBig);
	}
}

int YpVoip::cropVideoRawNV12(int w, int h, uint8_t* videoData, int dataLen, int yuvProcessPlan, int rotation, int needMirror, uint8_t* outVideoDataBig, uint8_t* outVideoDataSmall)
{
	int ret = 0;
	if (m_pVoipManager != NULL)
	{
		ret = m_pVoipManager->cropVideoRawNV12(w, h, videoData, dataLen, (int)CROP_TYPE::VIDEO_CROP_CONFIG_480BW_480BH_SMALL_NONE, 0, 0, outVideoDataBig, outVideoDataSmall);
	}
	return ret;
}
void YpVoip::drawPic(YUV_TYPE type, int w, int h, uint8_t* videoData, int videoDataLen)
{
	if (m_ShowLiveDlg->m_pDataShowView != NULL && m_bAudio == false)
	{
		m_ShowLiveDlg->m_pDataShowView->drawPic(type, m_pUserManager->m_ServiceParam.m_strUserId, w, h, videoData, videoDataLen);
	}
}

void YpVoip::onNewMessage(CIMMessage* var1)
{
	CString strMsg = "";
	strMsg.Format("%s:%s", var1->m_strFromId.c_str(), var1->m_strContentData.c_str());
	//m_HistoryMsg.InsertString(0, strMsg);
}

void YpVoip::changeShowStyle(string strUserId)
{
	if (m_ShowLiveDlg->m_pDataShowView != NULL)
	{
		string strId = m_ShowLiveDlg->m_pDataShowView->changeShowStyle(strUserId, true);
		m_ShowLiveDlg->m_pDataShowView->changeShowStyle(strId, false);
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}
}
void YpVoip::closeCurrentLive()
{
}
void YpVoip::getLocalSoundData(char* pData, int nLength)
{
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->insertAudioRaw((uint8_t*)pData, nLength);
	}
}

void YpVoip::querySoundData(char** pData, int* nLength)
{
	if (m_pVoipManager != NULL)
	{
		m_pVoipManager->querySoundData((uint8_t**)pData, nLength);
	}
}



void YpVoip::callPerson(string targetid)
{
	bool isCallSuccess = false;
	//呼叫对方
	if (m_pVoipManager != NULL)
	{
		isCallSuccess=m_pVoipManager->call(targetid);
	}
	if (!isCallSuccess) {
		AfxMessageBox("呼叫失败");
		return;
	}
	m_ShowLiveDlg->ShowWindow(SW_SHOW);
	if (m_ShowLiveDlg->m_pDataShowView != NULL)
	{
		m_ShowLiveDlg->m_pDataShowView->addUser(m_pUserManager->m_ServiceParam.m_strUserId, true);
		m_ShowLiveDlg->m_pDataShowView->setShowPictures();
	}

	startGetData((CROP_TYPE)m_pUserManager->m_ServiceParam.m_CropType, false);
}
