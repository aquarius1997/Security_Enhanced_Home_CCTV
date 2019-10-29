# Security_Enhanced_Home_CCTV
## 졸업프로젝트 - 보안강화 홈CCTV

프로젝트와 관련된 자세한 사항(프로토콜 설명, 실행모습 등)은 '보안강화 홈 CCTV 포트폴리오.pdf'를 참고해 주시기 바랍니다.

### 프로젝트 목표

시중에서 판매되고 있는(2018-2019초) 가정용 홈CCTV는 실외에서도 실시간으로 집안의 상황을 볼 수 있어, 편리함이 중요시 되는 현재에 많은 인기를 끌고 있습니다.
하지만 홈CCTV를 공격해 다른 사람의 집안을 들여다 보는 등의 사건이 발생하여 많은 사람이 피해를 입었습니다.
따라서 본 프로젝트의 목적은 ID기반의 서명방식을 이용한 상호 인증 및 키교환 프로토콜(IBS)을 설계하여 제3의 외부공격자 뿐만 아니라 악의적인 의도를 가진 IP카메라 제조사로부터 안전한 홈CCTV르 설계하는것입니다.

-------------------------------

### 개발환경

* Raspberry Pi 3
* Android OS Device
* Servo motor
* Raspberry Pi camera
* Open SSL
* Android studio
* Gstreamer(미디어 스트리미 오픈소스 프레임워크)

--------------------------------

### 개발언어

:bangbang: ID기반 서명 및 키교환 프로토콜을 구성하는 네 가지 암호화 알고리즘은 C언어로 작성되었기 때문에 JNI(Java Native Interface)를 사용함
* Java
* C

--------------------------------

## 적용 알고리즘

1. Setup(1^n) -> (PP, msk) : IP카메라 제조사가 시스템 전첵 사용되느 공개파라미터(PP)와 마스터키(msk)를 생성

2. KeyGen(ID, msk) -> SKid : IP카메라 제조사가 msk로 IP카메라의 ID와 사용자의 ID에 대응하는 서명키를 생성 (SKclient는 애플리케이션을 다운로드할 때 애플리케이션 스토어엣 제공하는 보안채널을 통해 다운로드해야함)

3. Sign(M, SKid) -> message signature : 메세지(M)를 서명해 메세지서명을 출력

4. Verify(message signature, M, PP) -> True or False : 입력으로 받은 메세지서명이 메세지에 대응하는 것이 맞는지 검증. 올바를 경우 True를 출력


