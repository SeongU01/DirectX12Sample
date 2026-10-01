# 005.PBR

솔루션의 `005.PBR`을 시작 프로젝트로 선택한다. 기존 `001`~`004` 샘플은 그대로 유지된다.

## 엔진 / 클라이언트 경계

- 엔진 `AssetPaths`, `Texture2D`: 실행 파일 기준 경로 탐색, WIC 이미지 디코딩, 색 공간별 mipmap, GPU 업로드와 SRV 수명 관리.
- 엔진 `StaticMeshGeometry`: 위치/법선/접선(방향 부호 포함)/UV 정점과 구체·큐브 생성. 기존 GeometryGenerator 정점 형식은 유지한다.
- 엔진 `PbrMaterial`, `PbrLighting`: 재사용 가능한 재질·조명 데이터. `GraphicsCore`는 텍스처 로딩, 정적 메시 생성, 조명 설정을 제공한다.
- 엔진 `PbrRenderPass`: PSO·루트 시그니처·상수 버퍼·텍스처 바인딩과 실제 그리기. Renderer는 재질이 지정된 RenderItem을 이 패스로 전달한다.
- 클라이언트 `005.PBR`: 사용할 에셋, 카메라 초기 자세, 구체·큐브 배치, ImGui 위젯과 샘플 상태만 담당한다.

참고 프로젝트의 `RenderPass` / `ForwardPBRLitPass` 책임 분리와 Static/Skeletal VS 분리, 공통 BRDF 함수 구조를 현재 엔진에 맞게 적용했다. 전체 RenderScene·RenderTechnique·그림자·DXR 시스템을 복사하지 않았다.

## 셰이더 / 렌더링

`StaticMeshVS.hlsl`의 엔트리는 `StaticMeshVS`이다. 월드 좌표, 역전치 법선, 접선과 UV를 공통 `MeshToPixel` 구조로 출력한다. 나중에 스키닝 VS가 같은 출력을 만들면 PBR PS를 공유할 수 있다. 현재 본 데이터나 스키닝은 구현하지 않는다.

`PbrCommon.hlsli`는 프레임/오브젝트 상수와 VS-PS 계약, `PbrLighting.hlsli`는 GGX 분포·Smith 기하·Schlick Fresnel·에너지 분배를 담당한다. `PbrPS.hlsl`은 재질 맵과 방향광·거리 감쇠 점광원을 평가한 뒤 노출, Reinhard 톤매핑, sRGB 출력을 적용한다.

Albedo SRV만 sRGB이며 데이터 맵은 선형이다. 제공된 `normal-dx`를 사용하므로 G 채널을 뒤집지 않는다. AO는 간접광 근사 항에만 적용한다. Ambient는 일정한 확산광 근사이며 IBL이 아니다. 환경 반사·그림자·Height 변위/패럴랙스·Ray Tracing은 이번 샘플에 포함하지 않는다.

텍스처는 Albedo / Normal / Metallic / Roughness / AO 순서다. 누락된 슬롯에는 엔진 기본 텍스처를 사용하지만, 명시한 파일이 없거나 디코딩에 실패하면 오류로 알린다. 모든 GPU 리소스와 디스크립터는 사용 중인 프레임이 완료될 때까지 유지한다. 현재 Device가 매 프레임 GPU 완료를 기다리는 구조를 사용한다.

## 공통 상대경로

모든 프로젝트는 `GraphicsCore::LoadTexture("Texture/.../image.png", sRGB)` 또는 `AssetPaths::Resolve("Texture/.../image.png")`를 사용한다. 경로 기준은 `Asset` 폴더이며 `C:/...`와 `../`는 허용하지 않는다. 내부에서 OS 파일 접근을 위해 절대경로로 해석하는 것은 하드코딩과 다르다.

공통 빌드 타깃이 원본 Asset을 수정하지 않고 `bin/<Configuration>/Asset/`에 복사한다. 솔루션의 모든 앱이 이 위치를 공유한다. 로더는 실행 파일 옆 Asset을 우선하고, 배치되지 않았으면 실행 파일의 상위 디렉터리에서 찾는다. 현재 작업 디렉터리를 바꾸거나 Visual Studio 밖에서 실행해도 동일하다. 배포할 때 실행 파일과 `Asset`, `Shaders`, 필요한 DLL을 함께 전달한다.

사용 재질 경로는 `Texture/dark-grey-tiles-ue/dark-grey-tiles-ue/`이다. 원본 중첩 폴더와 Height/Preview 파일도 보존한다.

## 조작 / 검증

- 우클릭 유지: 시점 및 WASD/QE 이동. 놓기 또는 Esc: 커서 복원. ImGui가 입력을 점유하면 카메라는 반응하지 않는다.
- Metallic/Roughness는 텍스처 값에 곱하는 계수다. `Use texture maps`를 끄면 계수 자체로 재질을 비교할 수 있다.
- View는 Lit / Albedo / World normal / Metallic / Roughness / AO를 제공한다.
- Reset은 텍스처 핸들을 보존하면서 재질·조명·회전·View를 초기화한다.
- Debug x64 빌드 후 `tests\RunContracts.cmd`: 기존 렌더/FPS 테스트와 PBR 정점 프레임·상대경로 계약을 새 라이브러리에 대해 다시 컴파일하고 실행한다.
