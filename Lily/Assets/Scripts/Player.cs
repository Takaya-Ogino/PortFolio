using System.Collections;
using UnityEngine;

// プレイヤーキャラクターの操作機能を表します
public class Player : MonoBehaviour {
    #region アニメーション用変数/ 列挙体
    // 再生するアニメーションを指定します
    [SerializeField]
    private Object[] AnimationList;

    // アニメーションの種類を表します
    [SerializeField]
    enum AnimationPattern : int {
        Wait = 6,       // 待機（入力なし）
        Walk = 7,       // 歩き
        Push = 5,       // 箱を押す
        Pull = 4,       // 箱を引く
        Climb = 0,      // 梯子を上る
        Death = 1,      // 死亡（ゲームオーバー）
        Down = 2,       // 梯子を下る
        Jump = 3,       // ジャンプ
    }

    // プレイヤーのモーション状態を表します
    enum MotionState {
        Start,      // 開始前演出
        Wait,       // 待機
        Walk,       // 歩き
        Jump,       // ジャンプ／落下
        Grab,       // 箱を掴む
        Climb,      // 梯子の上り下り
        GameOver,   // ゲームオーバー演出中
        StageClear, // ステージクリアー演出中
    }
    [SerializeField]
    MotionState motionState = MotionState.Start;

    // キャラクター位置座標のオフセットを指定します
    [SerializeField]
    private Vector3 positionOffset = Vector3.zero;
    // キャラクターのスケールを指定します
    [SerializeField]
    private Vector3 characterScale = Vector3.one;

    // SpriteStudio Anime を操作するためのクラス
    Script_SpriteStudio6_Root scriptRoot = null;
    #endregion

    // プレイヤーが侵入できるx座標の最小値（ステージ開始時のプレイヤーの位置）を指定します
    private float minX;
    // プレイヤーが侵入できるx座標の最大値（ゴールの座標から少し進んだ地点）を指定します
    private float maxX;
    // 歩行時の速度を指定します
    [SerializeField]
    private Vector3 walkSpeed = new Vector3(2, 0, 0);
    // ジャンプ力を指定します
    [SerializeField]
    private Vector2 jumpForce = new Vector2(0, 200);
    // 箱を掴んでいるときの速度を表示します
    [SerializeField]
    private Vector3 pushSpeed = new Vector3(1, 0, 0);
    // 梯子の上り下り速度を指定します
    [SerializeField]
    private float speedOnLadder = 1.5f;
    // 梯子のx座標の中央を指定します
    private float ladderPosX;
    // 落下開始地点のY座標を表します
    private float fallenPosition;
    // 落下速度を表します
    private float fallenSpeed;
    // 落ちた際にゲームオーバーになる落下速度を指定します
    [SerializeField]
    private float deadFallenSpeed = 100;

    // 接地しているかどうかのフラグ
    [SerializeField]
    private bool isGrounded;
    // 箱に接触しているか（掴めるか）どうかのフラグ
    private bool isGrabable;
    // 梯子に掴まっているかどうかのフラグ
    private bool isClimbing;

    // 再生する足音を指定します（0:草の上　1:レンガの上）
    [SerializeField]
    private AudioClip[] footStepSounds;
    // 落下した高さに応じた着地音を指定します(0:死亡しない高さ　1:死亡する高さ)
    [SerializeField]
    private AudioClip[] landingSounds;

    // ゴールを指定します
    private GameObject goal;

    #region レイヤーマスク
    // 地面判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask groundLayer;
    // 箱判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask boxLayer;
    // 梯子判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask ladderLayer;
    // 敵キャラクター判定用の対象レイヤーを指定します
    [SerializeField]
    private LayerMask enemyLayer;
    #endregion

    // 接地判定用のコライダーを指定します
    [SerializeField]
    private BoxCollider2D groundChecker = null;
    // 箱判定用のコライダーを指定します
    [SerializeField]
    private BoxCollider2D boxChecker = null;

    // コンポーネントを参照する変数
    new Rigidbody2D rigidbody2D;
    AudioSource audioSource;

    void Start() {
        // コンポーネントを取得
        rigidbody2D = GetComponent<Rigidbody2D>();
        audioSource = GetComponent<AudioSource>();

        //ステージに応じて再生する足音を設定します
        // 1,2ステージ目
        if (StageScene.StageNo == 0 || StageScene.StageNo == 1) {
            audioSource.clip = footStepSounds[0];
        }
        // 3ステージ目
        else {
            audioSource.clip = footStepSounds[1];
        }

        // ゴールをタグで検索します
        goal = GameObject.FindWithTag("Finish");

        // プレイヤーのX軸の行動範囲の下限と上限を設定します
        minX = transform.position.x;
        maxX = goal.transform.position.x + 20;

        //落下開始位置をステージ開始時の高さに設定します
        fallenPosition = transform.position.y;

        // アニメーションを開始します
        StartAnimation();
    }

    // 毎フレームに一度呼び出される更新処理を記述します
    void Update() {
        // プレイヤーの状態別に分岐処理
        switch (motionState) {
            case MotionState.Start:
                UpdateForStartState();
                break;
            case MotionState.Wait:
                UpdateForWaitState();
                break;
            case MotionState.Walk:
                UpdateForWalkState();
                break;
            case MotionState.Jump:
                UpdateForJumpState();
                break;
            case MotionState.Grab:
                UpdateForGrabState();
                break;
            case MotionState.Climb:
                UpdateForClimbState();
                break;
            case MotionState.GameOver:
                break;
            case MotionState.StageClear:
                UpdateForStageClearState();
                break;
            default:
                break;
        }
        // プレイヤーの行動範囲制限
        Clamp();
    }

    // プレイヤーの行動範囲を制限します
    private void Clamp() {
        var position = transform.position;
        // プレイヤーのx座標の範囲を制限
        position.x = Mathf.Clamp(position.x, minX, maxX);
        transform.position = position;
    }

    #region ステート遷移
    // Waitステートに遷移させます
    private void SetWaitState() {
        motionState = MotionState.Wait;
        ChangeAnimation(AnimationPattern.Wait);
    }

    // Walkステートに遷移させます
    private void SetWalkState() {
        audioSource.Play();
        motionState = MotionState.Walk;
        ChangeAnimation(AnimationPattern.Walk);
    }

    // Jumpステートに遷移させます
    private void SetJumpState() {
        audioSource.Stop();
        rigidbody2D.AddForce(jumpForce, ForceMode2D.Impulse);
        motionState = MotionState.Jump;
        ChangeAnimation(AnimationPattern.Jump);
    }

    //Grabステートに遷移させます
    private void SetGrabState() {
        motionState = MotionState.Grab;
    }

    // Climbステートに遷移させます
    private void SetClimbState() {
        var vertical = Input.GetAxis("Vertical");
        if (vertical > 0) {
            ChangeAnimation(AnimationPattern.Climb);
        }
        else if (vertical < 0) {
            ChangeAnimation(AnimationPattern.Down);
        }
        motionState = MotionState.Climb;
        transform.position = new Vector3(ladderPosX, transform.position.y, 0);
        // 梯子を上っている間は重力の影響をなくします
        rigidbody2D.gravityScale = 0;
        isClimbing = true;
    }

    // Gameoverステートに遷移させます
    private void SetGameOverState() {
        StartCoroutine(OnGameOver());
    }

    // StageClearステートに遷移させます
    private void SetStageClearState() {
        motionState = MotionState.StageClear;
        audioSource.Stop();
        StartCoroutine(OnStageClear());
    }
    #endregion

    #region
    // ゲームオーバー時の演出処理
    IEnumerator OnGameOver() {
        ChangeAnimation(AnimationPattern.Death);
        motionState = MotionState.GameOver;

        // 倒れるモーションに合わせて音声を再生します
        yield return new WaitForSeconds(1.4f);
        audioSource.PlayOneShot(landingSounds[1]);

        // 1,5秒後にゲームオーバー画面へ移行します
        yield return new WaitForSeconds(1.5f);
        StageScene.Instance.GameOver();

        yield return null;
    }

    // ステージクリア時の演出処理
    IEnumerator OnStageClear() {
        ChangeAnimation(AnimationPattern.Walk);
        audioSource.Play();

        // 1.5秒後に次のステージへ移行します
        yield return new WaitForSeconds(1.5f);

        StageScene.Instance.StageClear();

        yield return null;
    }
    #endregion

    #region フレーム更新処理
    // Startステートの際のフレーム更新処理です
    private void UpdateForStartState() {
        SetWaitState();
    }

    // Waitステートの際のフレーム更新処理です
    private void UpdateForWaitState() {
        // 歩き
        if (Input.GetButton("Horizontal")) {
            SetWalkState();
        }

        // ジャンプ
        if (Input.GetButtonDown("Jump")) {
            SetJumpState();
        }

        // 箱を掴む
        if (Input.GetButtonDown("Fire1")) {
            if (isGrabable) {
                SetGrabState();
            }
        }

        // 梯子に掴まる
        if (Input.GetButtonDown("Vertical")) {
            if (isClimbing) {
                SetClimbState();
            }
        }
    }

    // Walkステートの際のフレーム更新処理です
    private void UpdateForWalkState() {
        var horizontal = Input.GetAxis("Horizontal");

        // ジャンプ
        if (Input.GetButtonDown("Jump")) {
            audioSource.Stop();
            SetJumpState();
        }

        // 箱を掴む
        else if (Input.GetButtonDown("Fire1")) {
            if (isGrabable) {
                audioSource.Stop();
                SetGrabState();
            }
        }

        // 左右入力をしている間移動
        if (Input.GetButton("Horizontal")) {
            if (horizontal > 0) {
                transform.position += walkSpeed * Time.deltaTime;
                transform.localScale = new Vector3(1, 1, 1);
            }
            else if (horizontal < 0) {
                transform.position += -walkSpeed * Time.deltaTime;
                transform.localScale = new Vector3(-1, 1, 1);
            }
        }
        else {
            audioSource.Stop();
            SetWaitState();
        }
    }

    // Jumpステートの際のフレーム更新処理です
    private void UpdateForJumpState() {
        // 左右入力をしている間移動
        if (Input.GetButton("Horizontal")) {
            //左右入力を判定する変数
            var horizontal = Input.GetAxis("Horizontal");
            if (horizontal < 0) {
                transform.position += -walkSpeed * Time.deltaTime;
                transform.localScale = new Vector3(-1, 1, 1);
            }
            else if (horizontal > 0) {
                transform.position += walkSpeed * Time.deltaTime;
                transform.localScale = new Vector3(1, 1, 1);
            }
        }
        // 空中にいる間は落下開始地点を更新します
        UpdateFallenPosition();
    }

    // Grabステートの際のフレーム更新処理です
    private void UpdateForGrabState() {
        // 左右入力の判定
        var horizontal = Input.GetAxis("Horizontal");

        // 箱との接触判定
        Vector2 point = boxChecker.transform.position;
        point += boxChecker.offset;
        point = boxChecker.transform.position;
        point += boxChecker.offset;
        var tuochBox = Physics2D.OverlapBox(
            point,
            boxChecker.size,
            boxChecker.transform.rotation.eulerAngles.z,
            boxLayer);

        if (tuochBox) {
            // 掴んでいる箱を指定します
            var box = tuochBox.gameObject;
            // 箱とプレイヤーの距離をジョイントで固定します
            box.GetComponent<FixedJoint2D>().connectedBody = this.rigidbody2D;
            box.GetComponent<Box>().beingGrabed = true;

            // 掴むボタンを離す or 箱が空中にあると箱を離します
            if (Input.GetButtonUp("Fire1") || box.GetComponent<Box>().isFloating) {
                // ジョイントを解除します
                box.GetComponent<Box>().beingGrabed = false;
            }
        }

        // プレイヤーの向きに応じて対応するアニメーションを再生します
        if (Input.GetButtonDown("Horizontal")) {
            audioSource.Play();
            // 右向きの場合
            if (transform.localScale.x > 0) {
                // 右入力で押す
                if (horizontal > 0) {
                    ChangeAnimation(AnimationPattern.Push);
                }
                // 左入力で引く
                else if (horizontal < 0) {
                    ChangeAnimation(AnimationPattern.Pull);
                }
            }
            // 左向きの場合
            else if (transform.localScale.x < 0) {
                // 右入力で引く
                if (horizontal > 0) {
                    ChangeAnimation(AnimationPattern.Pull);
                }
                // 左入力で押す
                else if (horizontal < 0) {
                    ChangeAnimation(AnimationPattern.Push);
                }
            }
        }

        // 左右ボタンを離したら足音を停止します
        if (Input.GetButtonUp("Horizontal")) {
            audioSource.Stop();
        }

        // 左右入力をしている間の移動処理
        if (Input.GetButton("Horizontal")) {
            if (horizontal > 0) {
                transform.position += pushSpeed * Time.deltaTime;
            }
            else if (horizontal < 0) {
                transform.position += -pushSpeed * Time.deltaTime;
            }
        }
        //Grabボタンを離すとWaitステートに遷移します
        if (Input.GetButtonUp("Fire1")) {
            SetWaitState();
        }
    }

    // Climbステートの際のフレーム更新処理です
    private void UpdateForClimbState() {
        // 上下どちらが入力をされているか判定する変数
        var vertical = Input.GetAxis("Vertical");
        var horizontal = Input.GetAxis("Horizontal");

        if (Input.GetButtonDown("Vertical")) {
            if (vertical > 0) {
                ChangeAnimation(AnimationPattern.Climb);
            }
            else if (vertical < 0) {
                ChangeAnimation(AnimationPattern.Down);
            }
        }
        if (Input.GetButton("Vertical")) {
            // 上が入力されると上る
            if (vertical > 0) {
                transform.position += new Vector3(0, speedOnLadder * Time.deltaTime, 0);
            }
            // 下が入力されると下る
            else if (vertical < 0) {
                transform.position += new Vector3(0, -speedOnLadder * Time.deltaTime, 0);
            }
        }
        // 梯子に掴まっている際にも横移動出来るようにします
        if (Input.GetButton("Horizontal")) {
            if (horizontal > 0) {
                transform.position += new Vector3(speedOnLadder * Time.deltaTime, 0, 0);
            }
            else if (horizontal < 0) {
                transform.position += new Vector3(-speedOnLadder * Time.deltaTime, 0, 0);
            }
        }
    }

    // StageClearStateの際のフレーム更新処理です
    private void UpdateForStageClearState() {
        // 接地している時に自動で歩かせ続けます
        if (isGrounded) {
            var velocity = rigidbody2D.velocity;
            velocity = walkSpeed;
            rigidbody2D.velocity = velocity;
        }
    }
    #endregion

    // 固定フレームレートで呼び出される更新処理を記述します
    void FixedUpdate() {
        // 地面との交差判定
        Vector2 point = groundChecker.transform.position;
        point += groundChecker.offset;
        var result = Physics2D.OverlapBox(
            point,
            groundChecker.size,
            groundChecker.transform.rotation.eulerAngles.z,
            groundLayer);
        // 交差判定に応じて接地判定を切り替えます
        if (result) {
            isGrounded = true;
        }
        else {
            isGrounded = false;
        }
        // motionStateと接地判定に応じて処理を行います
        switch (motionState) {
            case MotionState.Wait:
                if (!isGrounded) {
                    SetJumpState();
                }
                break;
            case MotionState.Walk:
                if (!isGrounded) {
                    SetJumpState();
                }
                break;
            case MotionState.Jump:
                // 今回着地した場合
                if (isGrounded) {
                    audioSource.PlayOneShot(landingSounds[0]);
                    SetWaitState();
                    // 着地時に落下速度を計算します
                    CalculateFallenSpeed();
                    // 着地時に既定の速度を超えていたらゲームオーバーになります
                    if (fallenSpeed > deadFallenSpeed) {
                        audioSource.PlayOneShot(landingSounds[1]);
                        SetGameOverState();
                    }
                }
                break;
            case MotionState.Grab:
                if (!isGrounded) {
                    SetJumpState();
                }
                if (!isGrabable) {
                    SetWaitState();
                }
                break;
            case MotionState.Climb:
                if (!isGrounded) {
                    // 梯子から外れた時のために落下開始を現在地に更新します
                    fallenPosition = transform.position.y;
                }
                if (!isClimbing) {
                    SetWaitState();
                    rigidbody2D.gravityScale = 1;
                }
                break;
            default:
                break;
        }
    }

    #region 落下時処理
    // 落下開始地点を更新します
    private void UpdateFallenPosition() {
        // ジャンプで上に移動した際のことを考慮し、落下開始地点を最高地点に設定します
        fallenPosition = Mathf.Max(transform.position.y, fallenPosition);
    }

    // 落下速度を計算します
    private void CalculateFallenSpeed() {
        // 落下距離を経過フレームで割ることで速度を求めます
        fallenSpeed = ((fallenPosition - transform.position.y) / Time.deltaTime);
        // 速度計算後に落下開始位置を現在地の高さに更新します
        fallenPosition = transform.position.y;
    }
    #endregion

    // プレイヤーが他のオブジェクトのトリガーに侵入した際に呼び出されます
    private void OnTriggerEnter2D(Collider2D collision) {
        // 侵入したトリガーのレイヤー値
        var layerID = 1 << collision.gameObject.layer;

        // ゴール判定
        if (collision.CompareTag("Finish")) {
            audioSource.Stop();
            // カメラの追従を切ります
            ChaseCamera.IsChasing = false;
            SetStageClearState();
        }
        //梯子判定
        else if ((layerID & ladderLayer) != 0) {
            isClimbing = true;
            ladderPosX = collision.transform.position.x;
        }
        // 敵に触れたらゲームオーバー時の演出を流す
        else if ((layerID & enemyLayer) != 0) {
            audioSource.Stop();
            SetGameOverState();
        }

        // 箱との接触判定
        Vector2 point = boxChecker.transform.position;
        point += boxChecker.offset;
        var touchBox = Physics2D.OverlapBox(
            point,
            boxChecker.size,
            boxChecker.transform.rotation.eulerAngles.z,
            boxLayer);

        // 接触している間は箱を掴めるようにします
        if (touchBox) {
            isGrabable = true;
        }
    }

    // プレイヤーが他のオブジェクトのトリガーから出た際に呼び出されます
    private void OnTriggerExit2D(Collider2D collision) {
        // 侵入していたトリガーのレイヤー値
        var layerID = 1 << collision.gameObject.layer;
        // 梯子判定
        if ((layerID & ladderLayer) != 0) {
            isClimbing = false;
            ladderPosX = 0;
        }

        // 箱との接触判定
        Vector2 box = boxChecker.transform.position;
        box += boxChecker.offset;
        var touchBox = Physics2D.OverlapBox(
            box,
            boxChecker.size,
            boxChecker.transform.rotation.eulerAngles.z,
            boxLayer);

        // 接触していない間は箱を掴めないようにします
        if (!touchBox) {
            isGrabable = false;
        }
    }

    #region アニメーション用関数
    // アニメーション開始
    private void StartAnimation() {
        // 設定されたプレハブアニメーションの数
        int listLength = AnimationList.Length;

        // すでにアニメーション生成済 or リソース設定が無い場合はreturn
        if (scriptRoot != null || listLength < 1)
            return;

        // 再生するリソース名をリストから取得して再生する
        Object resourceObject = AnimationList[0];
        if (resourceObject != null) {
            // アニメーションを実体化
            var characterGameObject = Instantiate(resourceObject, Vector3.zero, Quaternion.identity) as GameObject;
            if (characterGameObject != null) {
                scriptRoot = Script_SpriteStudio6_Root.Parts.RootGet(characterGameObject);
                if (scriptRoot != null) {
                    // 座標設定するためのGameObject作成
                    var characterRoot = new GameObject("Lily");
                    if (characterRoot == null) {
                        // 作成できないケース対応
                        Destroy(characterGameObject);
                        scriptRoot = null;
                    }
                    else {
                        // 座標設定
                        characterGameObject.transform.SetParent(characterRoot.transform);

                        // 自分の子に移動して座標を設定
                        characterRoot.transform.SetParent(transform);
                        characterRoot.transform.localPosition = positionOffset;
                        characterRoot.transform.localRotation = Quaternion.identity;
                        characterRoot.transform.localScale = characterScale;

                        //アニメーション再生
                        ChangeAnimation(AnimationPattern.Wait);
                    }
                }
            }
        }
    }

    // アニメーション 再生/変更 
    private void ChangeAnimation(AnimationPattern pattern) {
        // アニメーションが生成されていなければreturn
        if (scriptRoot == null) {
            return;
        }

        // 再生する回数を指定します
        int timesPlay = 0;
        // 再生開始時のframeoffset（今回は0（初めから）で固定）
        int frameOffset = 0;
        // アニメーションの再生速度を指定します(1で標準)
        float rateTime = 1;
        // アニメーション生成済みなら受け取ったAnimationPatternごとに変数を設定
        if (scriptRoot != null) {
            switch (pattern) {
                case AnimationPattern.Wait:
                    timesPlay = 0;    // ループ再生 ;
                    break;
                case AnimationPattern.Walk:
                    timesPlay = 0;    // ループ再生 
                    rateTime = 1.5f;    // 再生速度を1.5倍にします
                    break;
                case AnimationPattern.Jump:
                    timesPlay = 1;    // 1回だけ再生 
                    break;
                case AnimationPattern.Push:
                    timesPlay = 0;    // ループ再生 
                    break;
                case AnimationPattern.Pull:
                    timesPlay = 0;    // ループ再生 
                    break;
                case AnimationPattern.Climb:
                    timesPlay = 0;    // ループ再生 
                    rateTime = 1.8f;    // 再生速度を1.8倍にします
                    break;
                case AnimationPattern.Down:
                    timesPlay = 0;    // ループ再生 
                    rateTime = 1.8f;    // 再生速度を1.8倍にします
                    break;
                case AnimationPattern.Death:
                    timesPlay = 1;    // 1回だけ再生 
                    break;
                default:
                    break;
            }
            // 設定した値を使用してアニメーションを再生する
            scriptRoot.AnimationPlay(-1, (int)pattern, timesPlay, frameOffset, rateTime);
        }
    }
    #endregion
}

