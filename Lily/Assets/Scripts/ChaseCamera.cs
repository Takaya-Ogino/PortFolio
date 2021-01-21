using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// カメラをプレイヤーに追従させます
public class ChaseCamera : MonoBehaviour {
    // プレイヤーの位置座標
    [SerializeField]
    private Transform player = null;

    // カメラが追従するかどうかを判定するフラグ
    public static bool IsChasing {
        get { return isChasing; }
        set { isChasing = value; }
    }
    static bool isChasing;

    // Start is called before the first frame update
    void Start() {
        // 開始時は追従するように設定します
        isChasing = true;
    }

    // Update is called once per frame
    void Update() {
        if (isChasing) {
            // プレイヤーのx座標を追尾する
            var position = transform.position;
            position.x = player.position.x;
            position.y = player.position.y + 3f;
            transform.position = position;
        }
    }
}
